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
#include <core/systems/gkc_collision_system.h>

namespace Galaktic::Core::Systems {
    /**
     * @class PhysicsSystem
     * @brief Integrates top-down bodies and coordinates non-rotating box collisions
     */
    class PhysicsSystem final : public BaseSystem {
        public:
            /** @param gravity Acceleration along positive Y; zero for top-down scenes */
            explicit PhysicsSystem(float gravity = 0.f) : m_gravity(gravity) {}
            /** @brief Applies forces, moves bodies, resolves contacts and clears forces */
            void Update(const std::unordered_map<EntityID, ECS::Entity>& list, float dt) override;
            /** @brief Returns collision queries and events from the last fixed tick */
            CollisionSystem& GetCollisionSystem() { return m_collisionSystem; }
        private:
            float m_gravity;
            CollisionSystem m_collisionSystem;
            /** @brief Integrates one body's velocity and world position */
            void IntegrateMotion(ECS::Entity entity, float dt);
            /** @brief Sweeps a dynamic body against static walls and slides along them */
            void MoveBody(ECS::TransformComponent& transform, ECS::RigidBody& body,
                const ECS::CollisionComponent& collider, EntityID id, float dt);
    };
}
