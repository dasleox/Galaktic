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
#include <ecs/gkc_registry.h>
#include "ecs/gkc_entity.h"
#include <ecs/gkc_components.h>

#include "core/systems/gkc_key.h"

namespace Galaktic::Core::Managers {
    class ECS_Manager {
        public:
            ECS_Manager(ECS::Registry* registry) : m_registry(registry) {}

            /**
             * @brief Creates an entity with a name component and a tag
             * @tparam T Tag Component
             * @param name Name of the entity
             */
            template<typename T>
            void CreateEntity(const string& name) {
                // Entity ID's increased by the size of the list
                EntityID id = m_entityList.size() + 1;
                m_entityList.emplace(id, ECS::Entity(id, m_registry));

                // Add a name component to the entity
                m_registry->Add<ECS::NameComponent>(id, name);

                // Adds a Tag for the entity
                m_registry->Add<T>(id);
            }

            /**
             * @brief Gets a reference to the component of the entity by ID
             * @tparam T Component type
             * @param id Entity's ID
             * @return Returns a reference to the component's Entity
             */
            template<typename T>
            T& GetComponentOfEntity(EntityID id) {
                return m_registry->Get<T>(id);
            }

            /**
             * @brief Adds a component to the desired entity by ID
             * @tparam T Component type
             * @tparam Args Argument types
             * @param id Entity's ID
             * @param args Arguments of the component
             */
            template<typename T, typename... Args>
            void AddComponentToEntity(EntityID id, Args&& ... args) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;
                it->second.Add<T>(std::forward<Args>(args)...);
            }

            /**
             * @brief Deletes a component by the given entity's ID
             * @tparam T Component Typename
             * @param id Entity's ID
             */
            template<typename T>
            void DeleteComponentToEntity(EntityID id) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;

                if (it->second.Has<T>())
                    it->second.Remove<T>(id);
            }

            /**
             * @brief Deletes the entity by its ID
             * @param id Entity's ID
             */
            void DeleteEntity(EntityID id) {
                m_entityList.erase(id);
            }

            ECS::Entity_List& GetEntityList() { return m_entityList; }
        private:
            ECS::Entity_List m_entityList;
            ECS::Registry* m_registry;
    };
}
