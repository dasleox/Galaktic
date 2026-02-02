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
#include <ecs/gkc_components.h>
#include <core/gkc_logger.h>
#include "ecs/gkc_template_traits.h"

typedef Uint32 ComponentTypeID;

namespace Galaktic::ECS {

    /**
     * @class ComponentRegistry
     * @brief Register and retrieves components from entities
     *
     * This class registers or retrieves components of entities, each time a component is registered
     * is assigned inside the wrapper ComponentTypeInfo, this struct helps to write/read components
     * from an entity as well as define automatically the write/read behaviour IF the component
     * is not a POD struct (Plain Old Data)
     *
     */
    class ComponentRegistry {
        public:
            /**
             * @brief Wraps a component inside ComponentTypeInfo
             *
             * Wraps a component of template type Component, its defined a type_index,
             * the size of the actual Component, the entity's id which is from,
             * and if the entity is an POD/Tag and defines the lambdas for serializing
             * and deserializing and then emplaced in the component type's map of this
             * class
             * @tparam Component Component Type
             * @param id Entity's ID
             * @param isTag Boolean if is a Tag
             */
            template<typename Component>
            static void RegisterComponent(EntityID id, bool isTag = false) {
                const type_index typeIndex(typeid(Component));
                constexpr bool isPOD = std::is_trivially_copyable_v<Component>;

                if constexpr (IsTag<Component>) {
                    ComponentTypeInfo info(
                        typeIndex,
                        0,
                        id,
                        true,
                        false,
                        [](const any&) -> size_t { return 0ull; },
                        [](const any&, ofstream&) {},
                        [](any&, ifstream&) {}
                    );

                    m_compTypes.emplace(typeIndex, std::move(info));
                    return;
                }

                size_t sizeValue = 0;
                size_t (*sizeFn)(const any&) = nullptr;

                if constexpr (IsNonPOD<Component>) {
                    sizeFn = [](const any& a) -> size_t {
                        const Component& c = std::any_cast<const Component&>(a);
                        return Component::Size(c);
                    };
                } else {
                    sizeValue = sizeof(Component);
                    sizeFn = [](const any&) -> size_t {
                        return sizeof(Component);
                    };
                }

                auto serializeFn = [](const any& a, ofstream& file) {
                    const Component& c = std::any_cast<const Component&>(a);
                    if constexpr (isPOD) {
                        file.write(GKC_WRITE_BINARY(c), sizeof(Component));
                    } else {
                        Component::Write(file, c);
                    }
                };

                auto deserializeFn = [](any& a, ifstream& file) {
                    Component c{};
                    if constexpr (isPOD) {
                        file.read(GKC_READ_BINARY(c), sizeof(Component));
                    } else {
                        Component::Read(file, c);
                    }
                    a = std::move(c);
                };

                GKC_ASSERT(std::is_trivially_copyable_v<Component> ||
                    requires { Component::Size; Component::Write; Component::Read; },
                    "Non-POD components require a specialized Size, Write & Read function/s");

                ComponentTypeInfo info(
                    typeIndex,
                    sizeValue,
                    id,
                    isTag,
                    isPOD,
                    sizeFn,
                    serializeFn,
                    deserializeFn
                );

                m_compTypes.emplace(typeIndex, info);
            }

            static const ComponentTypeInfo& Get(const type_index& type) {
                return m_compTypes.find(type)->second;
            }

            static void RegisterComponentByType(const type_index& type, EntityID id, bool isTag) {
                auto it = m_compTypes.find(type);
                if (it == m_compTypes.end())
                    GKC_ASSERT(false, "Attempted to deserialize unregistered component type");
            }
            static bool IsRegistered(const type_index& type) {
                return m_compTypes.contains(type);
            }

            static unordered_map<type_index, ComponentTypeInfo>& GetComponentTypes() {
                return m_compTypes;
            }
        private:
            static unordered_map<type_index, ComponentTypeInfo> m_compTypes;
    };
}

