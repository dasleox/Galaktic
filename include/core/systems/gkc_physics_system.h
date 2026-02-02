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
#include <pch.hpp>
#include <core/systems/gkc_system.h>

namespace Galaktic::ECS {
    class Entity;
}

namespace Galaktic::Core::Systems {
    /**
     * @class PhysicsSystem
     * @brief Manages the physics of entities that have physics components
     * 
     * Manages all physics of entities that have physics components, applying forces,
     * integrating motion, and resolving collisions.
     * A custom gravity can be applied to the whole scene, it will affect all entities
     * inside the scene that have physics components.
     */
    class PhysicsSystem final : public BaseSystem {
        public:
            /**
             * @param gravity Gravity applied to the scene
             */
             explicit PhysicsSystem(float gravity = -9.81f)
                    : m_gravity(gravity) {}

            /**
             * @brief Applies physics to entities
             * @param list List of entities
             * @param dt Delta time
             */
            void Update(const unordered_map<EntityID, ECS::Entity>& list, float dt) override;
        private:
            /**
             * @brief Apply forces to entities that have physics components
             * @param list List of entities
             * @note It only applies gravity force by now
             */
            void ApplyForces(const unordered_map<EntityID, ECS::Entity>& list) const;

            /**
             * @brief Integrate velocity to all physics component entities
             * @param list List of entities
             * @param dt Delta time
             */
            void IntegrateMotion(const unordered_map<EntityID, ECS::Entity>& list, float dt);

            /**
             * @brief Clean all forces
             * @param list List of entities
             */
            void CleanForces(const unordered_map<EntityID, ECS::Entity>& list);

            /**
             * @brief Apply jump force to the player entity
             * @param entity Player entity
             */
            void ApplyJump(ECS::Entity& entity);
            // void CollideEntities(const vector<ECS::Entity>& list, float dt);

            /**
             * @brief Applies collision if the entities touch the floor of the scene (y = 0.0f)
             * @param list List of entities
             */
            void ResolveGroundCollision(const unordered_map<EntityID, ECS::Entity>& list);
            float m_gravity;
    };
}