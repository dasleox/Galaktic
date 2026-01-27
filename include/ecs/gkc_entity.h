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

typedef Uint32 EntityID;
namespace Galaktic::Core {
    class Scene;
}

namespace Galaktic::ECS {

    /**
     * @class Entity
     * @brief Entity of the scene which contains modifiable-removable components
     */
    class Entity {
        public:
            Entity() = default;

            /**
             * @param id Entity ID
             * @param registry Registry pointer
             */
            Entity(EntityID id, Registry* registry): m_ID(id), m_registry(registry) {}

            [[nodiscard]] EntityID GetID() const { return m_ID; }
            [[nodiscard]] bool IsValid() const { return m_ID != InvalidEntity; }

            void SetID(EntityID id) { m_ID = id; }
            // Components Interface

            template<typename T, typename... Args>
            T& Add(Args&&... args) {
                return m_registry->Add<T>(
                    m_ID, std::forward<Args>(args)...);
            }

            template<typename T>
            T& Get() {
                return m_registry->Get<T>(m_ID);
            }

            template<typename T>
            [[nodiscard]] bool Has() const {
                return m_registry->Has<T>(m_ID);
            }

            template<typename T>
            void Remove() const {
                m_registry->Remove<T>(m_ID);
            }

            // Comparison Operators
            bool operator==(const Entity& other) const {
                return m_ID == other.m_ID && m_registry == other.m_registry;
            }

            bool operator!=(const Entity& other) const {
                return !(*this == other);
            }

        private:
            EntityID m_ID = InvalidEntity;
            Registry* m_registry= nullptr;
    };

    typedef unordered_map<EntityID, Entity> Entity_List;

    // Searching by name helper list
    typedef unordered_multimap<string, EntityID> NameToEntity_List;
}