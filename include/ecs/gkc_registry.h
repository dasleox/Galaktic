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

namespace Galaktic::Core {
    class Scene;
}

typedef Uint32 EntityID;

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

        private:
            unordered_map<type_index,unordered_map<EntityID, any>> m_componentPools;
    };
}