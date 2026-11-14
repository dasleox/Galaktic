#include <core/systems/gkc_collision_system.h>
#include <ecs/gkc_entity.h>
#include <algorithm>
#include <cmath>
#include <climits>
#include <limits>

using namespace Galaktic;
using namespace Galaktic::ECS;
using namespace Galaktic::Core::Systems;
using Render::Vec2;

using std::min;
using std::max;
using std::abs;
using std::vector;
using std::unordered_map;
using std::clamp;
using std::floor;
using std::isfinite;
using std::numeric_limits;
using std::sort;
using std::sqrt;
using std::swap;
using std::unique;

namespace {
    constexpr float ContactTolerance = 0.001f;
    constexpr float PenetrationSlop = 0.0001f;

    bool IsFinite(Vec2 value) {
        return isfinite(value.x) && isfinite(value.y);
    }

    bool IsValidQueryBox(Vec2 position, Vec2 size) {
        return IsFinite(position) && IsFinite(size) && size.x >= 0.f && size.y >= 0.f;
    }

    bool MasksMatch(Uint32 categoryA, Uint32 maskA, Uint32 categoryB, Uint32 maskB) {
        return (categoryA & maskB) && (categoryB & maskA);
    }

    bool HasDynamicMass(const RigidBody* body) {
        if (!body || body->bodyType != BodyType::Dynamic) return false;
        return isfinite(body->mass) && body->mass > 0.f;
    }

    Vec2 BodyVelocity(const RigidBody* body) {
        if (!body || body->bodyType == BodyType::Static) return {};
        return body->velocity;
    }

    bool IsSweepHit(float entry, float exit, float fraction) {
        bool pathsMiss = entry > exit;
        bool outsideMovement = exit < 0.f || entry < 0.f || entry > fraction;
        return !pathsMiss && !outsideMovement;
    }

    double CellCount(int left, int top, int right, int bottom) {
        // Include both end cells. Convert before subtraction to avoid integer overflow.
        return (double(right) - left + 1) * (double(bottom) - top + 1);
    }

    Uint64 PairKey(EntityID a, EntityID b) {
        // Sorting IDs makes A-B and B-A share one 64-bit key.
        return (Uint64(min(a, b)) << 32) | max(a, b);
    }

    Uint64 CellKey(int x, int y) {
        return (Uint64(Uint32(x)) << 32) | Uint32(y);
    }

    float Dot(Vec2 a, Vec2 b) {
        return a.x * b.x + a.y * b.y;
    }

    // Clamp cell coordinates before conversion to keep large world queries defined.
    int Cell(float value, float size) {
        double cell = floor(double(value) / size);
        return int(clamp(cell, double(INT_MIN) + 1, double(INT_MAX) - 1));
    }

    bool Overlaps(Vec2 a, Vec2 sizeA, Vec2 b, Vec2 sizeB) {
        bool overlapsX = a.x <= b.x + sizeB.x && a.x + sizeA.x >= b.x;
        bool overlapsY = a.y <= b.y + sizeB.y && a.y + sizeA.y >= b.y;
        return overlapsX && overlapsY;
    }

    bool Sweep(Vec2 position, Vec2 size, Vec2 delta, Vec2 target, Vec2 targetSize,
        float& fraction, Vec2& normal) {
        // Expand the target by the moving box size, then sweep its top-left point.
        // Each axis supplies an entry/exit interval measured as a fraction of delta.
        float entry = -numeric_limits<float>::infinity();
        float exit = numeric_limits<float>::infinity();
        for (int axis = 0; axis < 2; ++axis) {
            float origin = axis == 0 ? position.x : position.y;
            float movement = axis == 0 ? delta.x : delta.y;
            float low = axis == 0 ? target.x - size.x : target.y - size.y;
            float high = axis == 0 ? target.x + targetSize.x : target.y + targetSize.y;
            // A parallel path must lie inside this axis slab to hit the other axis.
            if (movement == 0.f) {
                if (origin <= low || origin >= high) return false;
                continue;
            }
            float entryTime = (low - origin) / movement;
            float exitTime = (high - origin) / movement;
            if (entryTime > exitTime) swap(entryTime, exitTime);
            // The last axis entered determines the surface normal at impact.
            if (entryTime > entry) {
                entry = entryTime;
                normal = axis == 0 ? Vec2{movement > 0.f ? -1.f : 1.f, 0.f}
                    : Vec2{0.f, movement > 0.f ? -1.f : 1.f};
            }
            exit = min(exit, exitTime);
        }
        if (!IsSweepHit(entry, exit, fraction)) return false;
        fraction = entry;
        return true;
    }
}

CollisionSystem::CollisionSystem(float cellSize)
    : m_cellSize(isfinite(cellSize) && cellSize > 0.f ? cellSize : 64.f) {}

void CollisionSystem::Refresh(const unordered_map<EntityID, Entity>& list) {
    m_proxies.clear();
    m_proxies.reserve(list.size());
    for (auto [id, entity] : list) {
        if (!entity.Has<TransformComponent>() || !entity.Has<CollisionComponent>()) continue;
        auto& transform = entity.Get<TransformComponent>();
        auto& collider = entity.Get<CollisionComponent>();
        Vec2 position = transform.location + collider.collisionOffset;
        Vec2 size = collider.collisionBox;
        if (!collider.collidable || size.x <= 0.f || size.y <= 0.f) continue;
        if (!IsFinite(position) || !IsFinite(size)) continue;
        auto* body = entity.Has<RigidBody>() ? &entity.Get<RigidBody>() : nullptr;
        m_proxies.push_back({id, &transform, &collider, body, position, size});
    }
    sort(m_proxies.begin(), m_proxies.end(), [](const Proxy& a, const Proxy& b) {
        return a.id < b.id;
    });
    BuildPairs();
}

void CollisionSystem::BuildPairs() {
    m_grid.clear();
    m_large.clear();
    m_pairs.clear();
    for (size_t i = 0; i < m_proxies.size(); ++i) {
        const auto& proxy = m_proxies[i];
        int left = Cell(proxy.position.x, m_cellSize);
        int top = Cell(proxy.position.y, m_cellSize);
        int right = Cell(proxy.position.x + proxy.size.x, m_cellSize);
        int bottom = Cell(proxy.position.y + proxy.size.y, m_cellSize);
        // Oversized boxes use a fallback list instead of filling hundreds of cells.
        if (CellCount(left, top, right, bottom) > 256) {
            m_large.push_back(i);
            continue;
        }
        for (int x = left; x <= right; ++x) {
            for (int y = top; y <= bottom; ++y) {
                auto& bucket = m_grid[CellKey(x, y)];
                for (size_t other : bucket) AddPair(other, i);
                bucket.push_back(i);
            }
        }
    }
    for (size_t large : m_large) {
        for (size_t i = 0; i < m_proxies.size(); ++i) {
            if (large != i) AddPair(min(large, i), max(large, i));
        }
    }
    // A pair may share several cells; solve it only once per iteration.
    sort(m_pairs.begin(), m_pairs.end(), [](Pair a, Pair b) {
        return a.a == b.a ? a.b < b.b : a.a < b.a;
    });
    m_pairs.erase(unique(m_pairs.begin(), m_pairs.end(), [](Pair a, Pair b) {
        return a.a == b.a && a.b == b.b;
    }), m_pairs.end());
}

void CollisionSystem::AddPair(size_t a, size_t b) {
    const auto& first = m_proxies[a];
    const auto& second = m_proxies[b];
    if (!CanCollide(first, second)) return;
    m_pairs.push_back({a, b});
}

bool CollisionSystem::CanCollide(const Proxy& a, const Proxy& b) const {
    if (!MasksMatch(a.collider->category, a.collider->mask,
        b.collider->category, b.collider->mask)) return false;
    if (!BoundsNear(a, b)) return false;
    bool bothImmovable = InverseMass(a) == 0.f && InverseMass(b) == 0.f;
    bool hasSensor = a.collider->isSensor || b.collider->isSensor;
    return !bothImmovable || hasSensor;
}

bool CollisionSystem::BoundsNear(const Proxy& a, const Proxy& b) const {
    // Keep nearly touching boxes eligible so resting contacts survive rounding.
    bool separatedX = a.position.x > b.position.x + b.size.x + ContactTolerance ||
        b.position.x > a.position.x + a.size.x + ContactTolerance;
    bool separatedY = a.position.y > b.position.y + b.size.y + ContactTolerance ||
        b.position.y > a.position.y + a.size.y + ContactTolerance;
    return !separatedX && !separatedY;
}

bool CollisionSystem::CanCast(const Proxy& proxy, Uint32 mask, EntityID ignore,
    bool staticOnly, Uint32 category) const {
    if (proxy.id == ignore || proxy.collider->isSensor) return false;
    if (!MasksMatch(category, mask, proxy.collider->category, proxy.collider->mask)) return false;
    bool movingBody = proxy.body && proxy.body->bodyType != BodyType::Static;
    return !staticOnly || !movingBody;
}

float CollisionSystem::InverseMass(const Proxy& proxy) {
    if (!HasDynamicMass(proxy.body)) return 0.f;
    return 1.f / proxy.body->mass;
}

bool CollisionSystem::DetectCollision(const Proxy& a, const Proxy& b, CollisionContact& contact) const {
    Vec2 centerA = a.position + a.size * 0.5f;
    Vec2 centerB = b.position + b.size * 0.5f;
    float overlapX = min(a.position.x + a.size.x, b.position.x + b.size.x) -
        max(a.position.x, b.position.x);
    float overlapY = min(a.position.y + a.size.y, b.position.y + b.size.y) -
        max(a.position.y, b.position.y);
    if (overlapX < -ContactTolerance || overlapY < -ContactTolerance) return false;
    // Sum of half-sizes minus center distance gives the distance to an exit face.
    // Unlike intersection width, this also separates a box fully inside another.
    float penetrationX = (a.size.x + b.size.x) * 0.5f - abs(centerB.x - centerA.x);
    float penetrationY = (a.size.y + b.size.y) * 0.5f - abs(centerB.y - centerA.y);
    contact = {a.id, b.id, {}, max(0.f, min(penetrationX, penetrationY)),
        a.collider->isSensor || b.collider->isSensor};
    contact.normal = penetrationX < penetrationY ? Vec2{centerB.x >= centerA.x ? 1.f : -1.f, 0.f}
        : Vec2{0.f, centerB.y >= centerA.y ? 1.f : -1.f};
    return true;
}

void CollisionSystem::ResolveCollision(Proxy& a, Proxy& b, const CollisionContact& contact) {
    float inverseMassA = InverseMass(a);
    float inverseMassB = InverseMass(b);
    float inverseMassSum = inverseMassA + inverseMassB;
    if (contact.isSensor || inverseMassSum == 0.f) return;

    // Split separation by inverse mass: lighter bodies move more; static bodies stay put.
    float separation = max(contact.penetration - PenetrationSlop, 0.f);
    Vec2 correction = contact.normal * (separation / inverseMassSum);
    a.transform->location += correction * -inverseMassA;
    b.transform->location += correction * inverseMassB;
    a.position = a.transform->location + a.collider->collisionOffset;
    b.position = b.transform->location + b.collider->collisionOffset;

    // Project relative velocity onto the normal; a positive value means separation.
    Vec2 velocityA = BodyVelocity(a.body);
    Vec2 velocityB = BodyVelocity(b.body);
    Vec2 relativeVelocity = velocityB - velocityA;
    float normalSpeed = Dot(relativeVelocity, contact.normal);
    if (normalSpeed >= 0.f) return;
    float bounce = clamp(max(a.body ? a.body->restitution : 0.f,
        b.body ? b.body->restitution : 0.f), 0.f, 1.f);
    // Restitution adds bounce to the impulse needed to cancel inward motion.
    float impulse = -(1.f + bounce) * normalSpeed / inverseMassSum;
    Vec2 normalImpulse = contact.normal * impulse;
    if (inverseMassA > 0.f) a.body->velocity += normalImpulse * -inverseMassA;
    if (inverseMassB > 0.f) b.body->velocity += normalImpulse * inverseMassB;

    // Friction acts along the tangent and cannot exceed friction * normal impulse.
    Vec2 tangent{-contact.normal.y, contact.normal.x};
    float friction = sqrt(max(0.f, a.body ? a.body->friction : 0.f) *
        max(0.f, b.body ? b.body->friction : 0.f));
    float frictionImpulse = clamp(-Dot(relativeVelocity, tangent) / inverseMassSum,
        -impulse * friction, impulse * friction);
    if (inverseMassA > 0.f) a.body->velocity += tangent * (-frictionImpulse * inverseMassA);
    if (inverseMassB > 0.f) b.body->velocity += tangent * (frictionImpulse * inverseMassB);
}

void CollisionSystem::Update(const unordered_map<EntityID, Entity>& list, float) {
    m_previous.swap(m_contacts);
    m_contacts.clear();
    for (const auto& contact : m_sweptContacts)
        m_contacts.try_emplace(PairKey(contact.entityA, contact.entityB), contact);
    m_sweptContacts.clear();
    Refresh(list);
    bool corrected = false;
    for (int iteration = 0; iteration < 6; ++iteration) {
        if (iteration > 0) BuildPairs();
        corrected = false;
        for (auto pair : m_pairs) {
            auto& a = m_proxies[pair.a];
            auto& b = m_proxies[pair.b];
            CollisionContact contact;
            if (!DetectCollision(a, b, contact)) continue;
            m_contacts.try_emplace(PairKey(a.id, b.id), contact);
            corrected |= !contact.isSensor && contact.penetration > PenetrationSlop;
            ResolveCollision(a, b, contact);
        }
        if (!corrected) break;
    }
    if (corrected) BuildPairs();
    BuildEvents();
}

void CollisionSystem::RecordSweep(EntityID entity, const CollisionHit& hit) {
    bool first = entity < hit.entity;
    m_sweptContacts.push_back({min(entity, hit.entity), max(entity, hit.entity),
        hit.normal * (first ? -1.f : 1.f), 0.f, false});
}

void CollisionSystem::BuildEvents() {
    m_events.clear();
    for (const auto& [key, contact] : m_contacts) {
        m_events.push_back({contact, m_previous.contains(key) ? CollisionState::Stay : CollisionState::Enter});
    }
    for (const auto& [key, contact] : m_previous) {
        if (!m_contacts.contains(key)) m_events.push_back({contact, CollisionState::Exit});
    }
    sort(m_events.begin(), m_events.end(), [](const CollisionEvent& a, const CollisionEvent& b) {
        return PairKey(a.contact.entityA, a.contact.entityB) < PairKey(b.contact.entityA, b.contact.entityB);
    });
}

vector<size_t> CollisionSystem::QueryCandidates(Vec2 position, Vec2 size) const {
    vector<size_t> result;
    if (!IsValidQueryBox(position, size)) return result;
    int left = Cell(position.x, m_cellSize), top = Cell(position.y, m_cellSize);
    int right = Cell(position.x + size.x, m_cellSize), bottom = Cell(position.y + size.y, m_cellSize);
    if (CellCount(left, top, right, bottom) > 4096) {
        for (size_t i = 0; i < m_proxies.size(); ++i) result.push_back(i);
        return result;
    }
    result = m_large;
    for (int x = left; x <= right; ++x) {
        for (int y = top; y <= bottom; ++y) {
            auto it = m_grid.find(CellKey(x, y));
            if (it != m_grid.end()) result.insert(result.end(), it->second.begin(), it->second.end());
        }
    }
    sort(result.begin(), result.end());
    result.erase(unique(result.begin(), result.end()), result.end());
    return result;
}

vector<EntityID> CollisionSystem::OverlapBox(Vec2 position, Vec2 size, Uint32 mask) const {
    vector<EntityID> result;
    for (size_t i : QueryCandidates(position, size)) {
        const auto& proxy = m_proxies[i];
        if ((proxy.collider->category & mask) && Overlaps(position, size, proxy.position, proxy.size))
            result.push_back(proxy.id);
    }
    return result;
}

CollisionHit CollisionSystem::CastBox(Vec2 position, Vec2 size, Vec2 displacement, Uint32 mask,
    EntityID ignore, bool staticOnly, Uint32 category) const {
    CollisionHit hit;
    if (!IsFinite(displacement)) return hit;
    // Query every cell touched between the starting and ending box positions.
    Vec2 start{min(position.x, position.x + displacement.x),
        min(position.y, position.y + displacement.y)};
    Vec2 bounds{size.x + abs(displacement.x), size.y + abs(displacement.y)};
    for (size_t i : QueryCandidates(start, bounds)) {
        const auto& proxy = m_proxies[i];
        if (!CanCast(proxy, mask, ignore, staticOnly, category)) continue;
        float fraction = hit.fraction;
        Vec2 normal;
        if (Sweep(position, size, displacement, proxy.position, proxy.size, fraction, normal))
            hit = {proxy.id, normal, fraction};
    }
    return hit;
}

CollisionHit CollisionSystem::Raycast(Vec2 origin, Vec2 displacement, Uint32 mask, EntityID ignore) const {
    return CastBox(origin, {}, displacement, mask, ignore);
}
