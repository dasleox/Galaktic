/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#include <core/systems/gkc_system.h>
#include <ecs/gkc_components.h>

namespace Galaktic::Core::Systems {
    using std::vector;
    using std::unordered_map;

    /** @brief Contact normal points from entity A towards entity B
     */
    struct CollisionContact {
        EntityID entityA = InvalidEntity;
        EntityID entityB = InvalidEntity;
        Render::Vec2 normal;
        float penetration = 0.f;
        bool isSensor = false;
    };

    enum class CollisionState { Enter, Stay, Exit };

    /** @brief Buffered contact event, valid until the next collision update
     */
    struct CollisionEvent {
        CollisionContact contact;
        CollisionState state = CollisionState::Enter;
    };

    /** @brief Nearest query hit; fraction is in the range [0, 1]
     */
    struct CollisionHit {
        EntityID entity = InvalidEntity;
        Render::Vec2 normal;
        float fraction = 1.f;
    };

    /** @brief Detects non-rotating boxes using a spatial hash and resolves solid contacts
     */
    class CollisionSystem final : public BaseSystem {
        public:
            /** @brief Creates the collision grid
             * @param cellSize World-space grid size, independent of collider size
             */
            explicit CollisionSystem(float cellSize = 64.f);

            /** @brief Detects, resolves and buffers events; do not mutate the ECS during this call
             * @param list Entity handles to inspect and resolve
             * @param dt Fixed timestep; unused by discrete detection
             */
            void Update(const unordered_map<EntityID, ECS::Entity>& list, float dt) override;

            /** @brief Refreshes query data after external transform or component changes
             * @param list Entity handles used to rebuild collider data
             */
            void Refresh(const unordered_map<EntityID, ECS::Entity>& list);

            /** @brief Returns events to consume after Update, before the next fixed tick
             * @return Contact events from the most recent update
             */
            const vector<CollisionEvent>& GetEvents() const { return m_events; }

            /** @brief Returns overlapping IDs; query boxes use a top-left position
             * @param position World-space top-left corner
             * @param size Query box dimensions
             * @param mask Accepted collider categories
             * @return Overlapping entity IDs, including sensors
             */
            vector<EntityID> OverlapBox(Render::Vec2 position, Render::Vec2 size,
                Uint32 mask = ~Uint32{0}) const;

            /** @brief Sweeps a box by displacement; sensors never block casts
             * @param position World-space top-left corner
             * @param size Moving box dimensions
             * @param displacement Movement vector including distance
             * @param mask Accepted collider categories
             * @param ignore Entity excluded from the cast
             * @param staticOnly Whether only static geometry can block the cast
             * @param category Category used to check target masks
             * @return Nearest hit; InvalidEntity when no box blocks movement
             */
            CollisionHit CastBox(Render::Vec2 position, Render::Vec2 size, Render::Vec2 displacement,
                Uint32 mask = ~Uint32{0}, EntityID ignore = InvalidEntity, bool staticOnly = false,
                Uint32 category = ~Uint32{0}) const;

            /** @brief Casts a point along displacement, using the same filtering as CastBox
             * @param origin World-space start point
             * @param displacement Ray direction including distance
             * @param mask Accepted collider categories
             * @param ignore Entity excluded from the ray
             * @return Nearest non-sensor hit
             */
            CollisionHit Raycast(Render::Vec2 origin, Render::Vec2 displacement,
                Uint32 mask = ~Uint32{0}, EntityID ignore = InvalidEntity) const;
                
        private:
            friend class PhysicsSystem;
            struct Proxy 
            {
                EntityID id;
                ECS::TransformComponent* transform;
                ECS::CollisionComponent* collider;
                ECS::RigidBody* body;
                Render::Vec2 position;
                Render::Vec2 size;
            };
            struct Pair { size_t a, b; };
            float m_cellSize;
            vector<Proxy> m_proxies;
            unordered_map<Uint64, vector<size_t>> m_grid;
            vector<size_t> m_large;
            vector<Pair> m_pairs;
            unordered_map<Uint64, CollisionContact> m_contacts;
            unordered_map<Uint64, CollisionContact> m_previous;
            vector<CollisionEvent> m_events;
            vector<CollisionContact> m_sweptContacts;

            /** @brief Preserves swept impacts even when a bounce separates the bodies
             * @param entity Moving entity
             * @param hit Static geometry hit during movement
             */
            void RecordSweep(EntityID entity, const CollisionHit& hit);

            /** @brief Rebuilds grid buckets and unique candidate pairs
             */
            void BuildPairs();

            /** @brief Collects unique proxies covered by a world-space query box
             * @param position World-space top-left corner
             * @param size Query box dimensions
             * @return Unique indices into the collider buffer
             */
            vector<size_t> QueryCandidates(Render::Vec2 position, Render::Vec2 size) const;

            /** @brief Adds a candidate only when masks and body types allow it
             * @param a First collider index
             * @param b Second collider index
             */
            void AddPair(size_t a, size_t b);

            /** @brief Checks masks, nearby bounds and body response rules
             * @param a First collider
             * @param b Second collider
             * @return Whether the pair needs a narrow-phase test
             */
            bool CanCollide(const Proxy& a, const Proxy& b) const;

            /** @brief Checks for nearby bounds using the resting-contact tolerance
             * @param a First world-space box
             * @param b Second world-space box
             * @return Whether both axis intervals are close enough to touch
             */
            bool BoundsNear(const Proxy& a, const Proxy& b) const;

            /** @brief Checks whether a collider can block a cast
             * @param proxy Candidate collider
             * @param mask Accepted categories
             * @param ignore Excluded entity
             * @param staticOnly Whether to reject moving bodies
             * @param category Category checked against the collider mask
             * @return Whether the candidate passes cast filtering
             */
            bool CanCast(const Proxy& proxy, Uint32 mask, EntityID ignore,
                bool staticOnly, Uint32 category) const;

            /** @brief Tests two world boxes without changing their bodies
             * @param a First world-space collider
             * @param b Second world-space collider
             * @param contact Contact data written when boxes touch
             * @return Whether boxes touch within contact tolerance
             */
            bool DetectCollision(const Proxy& a, const Proxy& b, CollisionContact& contact) const;
            
            /** @brief Corrects penetration and applies normal and friction impulses
             * @param a First collider and borrowed body data
             * @param b Second collider and borrowed body data
             * @param contact Detected normal and penetration
             */
            void ResolveCollision(Proxy& a, Proxy& b, const CollisionContact& contact);
            
            /** @brief Produces Enter, Stay and Exit events from canonical entity pairs
             */
            void BuildEvents();
            
            /** @brief Returns zero inverse mass for static and kinematic bodies
             * @param proxy Collider and optional body
             * @return Inverse dynamic mass, or zero for immovable bodies
             */
            static float InverseMass(const Proxy& proxy);
    };
}
