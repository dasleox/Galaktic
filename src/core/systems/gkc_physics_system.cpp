#include <core/systems/gkc_physics_system.h>
#include <ecs/gkc_entity.h>
#include <algorithm>
#include <cmath>

using namespace Galaktic;
using namespace Galaktic::ECS;
using namespace Galaktic::Core::Systems;

void PhysicsSystem::Update(const Entity_List& list, float dt) {
    if (!std::isfinite(dt) || dt <= 0.f) return;
    m_collisionSystem.Refresh(list);
    for (auto [id, entity] : list) {
        if (!entity.Has<RigidBody>()) continue;
        auto& body = entity.Get<RigidBody>();
        if (entity.Has<TransformComponent>()) IntegrateMotion(entity, dt);
        body.force = {};
    }
    m_collisionSystem.Update(list, dt);
}

void PhysicsSystem::IntegrateMotion(Entity entity, float dt) {
    auto& body = entity.Get<RigidBody>();
    auto& transform = entity.Get<TransformComponent>();
    if (body.bodyType == BodyType::Static) return;
    if (body.bodyType == BodyType::Dynamic) {
        if (!std::isfinite(body.mass) || body.mass <= 0.f) return;
        auto acceleration = body.force / body.mass;
        acceleration.y += m_gravity * body.gravityScale;
        body.velocity += acceleration * dt;
    }
    if (!std::isfinite(body.velocity.x) || !std::isfinite(body.velocity.y)) {
        body.velocity = {};
        return;
    }
    if (body.bodyType == BodyType::Dynamic && entity.Has<CollisionComponent>()) {
        auto& collider = entity.Get<CollisionComponent>();
        if (collider.collidable && !collider.isSensor &&
            collider.collisionBox.x > 0.f && collider.collisionBox.y > 0.f) {
            MoveBody(transform, body, collider, entity.GetID(), dt);
            return;
        }
    }
    transform.location += body.velocity * dt;
}

void PhysicsSystem::MoveBody(TransformComponent& transform, RigidBody& body,
    const CollisionComponent& collider, EntityID id, float dt) {
    float remaining = dt;
    for (int i = 0; i < 3 && remaining > 0.f; ++i) {
        auto displacement = body.velocity * remaining;
        auto hit = m_collisionSystem.CastBox(transform.location + collider.collisionOffset,
            collider.collisionBox, displacement, collider.mask, id, true, collider.category);
        transform.location += displacement * hit.fraction;
        if (hit.entity == InvalidEntity) break;
        m_collisionSystem.RecordSweep(id, hit);
        float speed = body.velocity.x * hit.normal.x + body.velocity.y * hit.normal.y;
        if (speed < 0.f)
            body.velocity += hit.normal * (-speed * (1.f + std::clamp(body.restitution, 0.f, 1.f)));
        remaining *= 1.f - hit.fraction;
    }
}
