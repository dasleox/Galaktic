/*
  Galaktic Engine
  Copyright (C) 2025 SummerChip

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
#include "core/managers/gkc_ecs_man.h"
#include "core/gkc_logger.h"

namespace Galaktic::ECS {
    class Entity;
}
namespace Galaktic::Core::Managers {
    class ECS_Manager;
}


namespace Galaktic::Core::Helpers {
    /**
     * @class ECS_Helper
     * @brief Helper to create entities directly without any overheads for the editor/user
     *
     * Highest layer of the ECS System, it creates entities using an entity manager, it
     * requires a previous registry and a component pool to work
     */
    class ECS_Helper {
        public:
            /**
             * @param escManager ECSManager instance
             */
            explicit ECS_Helper(Managers::ECS_Manager& escManager);

            /**
             * @brief Creates a player entity with default properties
             * @param name Name of the player
             * @note This function does not support multiplayer
             */
            void CreatePlayer(const string& name);

            /**
             * @brief Creates a static object with default properties
             * @param name Name of the object
             */
            void CreateStaticObject(const string& name);

            /**
             * @brief Creates a physics object with default properties
             * @param name Name of the object
             *
             * This creates an object with simple collision properties
             */
            void CreatePhysicsObject(const string& name);

            /**
             * @brief Creates a light entity
             * @param name Name of the light entity
             */
            void CreateLightEntity(const string& name);

            /**
             * @brief Creates a camera entity
             * @param name Name of the camera
             */
            void CreateCameraEntity(const string& name);

            /**
             * @brief Modifies a component based on the type, if the component doesn't exist in
             *        the entity, it will be added
             * @tparam Component Component type
             * @param name Name of the entity
             * @param component Component instance to change
             */
            template<typename Component>
            void ModifyEntity(const string& name, Component& component) {
                auto id = m_ecsManager.GetNameToEntityList().find(name)->second;
                if (m_ecsManager.GetEntityByName(name)->Has<Component>()) {
                    auto& oldComponent = m_ecsManager.GetComponentOfEntity<Component>(id);
                    oldComponent = component;
                }
                else {
                    GKC_ENGINE_INFO("Component {0} added to '{1}'",
                        Debug::Logger::DemangleTypename(typeid(Component).name())
                        , name);
                    m_ecsManager.AddComponentToEntity<Component>(id, component);
                }
            }

            [[nodiscard]] ECS::Entity& GetEntityByName(const string& name) const;
        private:
            Managers::ECS_Manager& m_ecsManager;
            EntityID m_id = InvalidEntity;
    };
}