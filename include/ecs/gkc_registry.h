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
#include "gkc_component_registry.h"

namespace Galaktic::Core {
    class Scene;
}

namespace Galaktic::ECS {
    /**
     * @class Registry
     * @brief Intermediary class between the ECS Manager and the Scene
     */
    class Registry {
        public:
            /**
             * @brief Adds a component to the specified entity by its id
             * @tparam T Component Type
             * @tparam Args Argument/s Type/s of the Component
             * @param id Entity's ID that gets added the component
             * @param args Arguments of the Component
             * @return Added component to the specified entity
             */
            template<typename T, typename... Args>
            T& Add(EntityID id, Args&&... args) {
                auto& any = m_componentPools[typeid(T)][id];
                any.emplace<T>(std::forward<Args>(args)...);
                return std::any_cast<T&>(any);
            }

            template<typename T>
            T& Add(EntityID id) {
                auto& any = m_componentPools[typeid(T)][id];
                any.emplace<T>();
                return std::any_cast<T&>(any);
            }

            /**
             * @brief Get a component from the specified entity
             * @tparam T Component Type
             * @param id Entity ID
             * @return The component from the entity
             */
            template<typename T>
            T& Get(EntityID id) {
                return std::any_cast<T&>(
                    m_componentPools[typeid(T)][id]
                );
            }

            /**
             * @brief Checks if the entity by ID has a specific component
             * @tparam T Component Type
             * @param id Entity's ID
             * @return true if the component exist, false otherwise
             */
            template<typename T>
            bool Has(EntityID id) const {
                auto it = m_componentPools.find(typeid(T));
                if (it == m_componentPools.end())
                    return false;

                return it->second.contains(id);
            }

            /**
             * @brief Removes a component from the entity's ID
             * @tparam T Component Type
             * @param id Entity's ID
             */
            template<typename T>
            void Remove(EntityID id) {
                m_componentPools[typeid(T)].erase(id);
            }

            unordered_map<type_index,unordered_map<EntityID, any>>& GetComponentPools() {
                return m_componentPools;
            }

            vector<type_index> GetComponentsFromEntity(EntityID id) {
                vector<type_index> components;

                for (const auto& [type, map] : m_componentPools) {
                    if (map.contains(id)) {
                        components.emplace_back(type);
                    }
                }

                return components;
            }

            /**
             * @brief Defines a lambda for all the entity components if the entity exists
             *
             * This function is used by the component registry, writer and reader to write
             * or read the components of an entity, determine the size of the entity, etc.
             * @param id Entity's ID
             * @param func Lambda Function
             */
            void ForEachComponentDo(EntityID id,
                                    const function<void(const ComponentTypeInfo&, const any&)> func) {
                for (const auto& [type, map] : m_componentPools) {
                    auto it = map.find(id);
                    if (it == map.end())
                        continue;

                    const ComponentTypeInfo& info = ComponentRegistry::Get(type);
                    func(info, it->second);
                }
            }
            void ForEachRegisteredComponent(const std::function<void(const ComponentTypeInfo&)>& fn) const {
                for (const auto& [type, info] : ComponentRegistry::GetComponentTypes()) {
                    fn(info);
                }
            }

            size_t GetAllComponentsSize(EntityID id) {
                size_t size = 0;

                for (const auto& [type, entityMap] : m_componentPools) {
                    auto it = entityMap.find(id);
                    if (it == entityMap.end())
                        continue;

                    const any& componentAny = it->second;
                    const ComponentTypeInfo& info = ComponentRegistry::Get(type);
                    size += info.size_func(componentAny);
                }
                return size;
            }

            template<typename T>
            bool IsComponentType(const type_index& type) {
                return type == std::type_index(typeid(T));
            }
        private:
            unordered_map<type_index,unordered_map<EntityID, any>> m_componentPools;
    };
}