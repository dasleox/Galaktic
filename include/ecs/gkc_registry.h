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
#include "gkc_component_registry.h"
#include "gkc_component_pool.h"

namespace Galaktic::Core {
    class Scene;
}
namespace Galaktic::ECS {

    /**
     * @class Registry
     * @brief Intermediary class between the ECS Manager and the Scene
     * 
     * This class manages all components for all entities in a scene.
     * It provides methods to add, get, remove, and iterate over components.
     * 
     * @see gkc_component_pool.h for details on the new pool system
     */
    class Registry {
        public:
            /**
             * @brief Default constructor
             */
            Registry() = default;
            
            /**
             * @brief Default destructor
             */
            ~Registry() = default;
            
            // Prevent copying (pools contain pointers/references)
            Registry(const Registry&) = delete;
            Registry& operator=(const Registry&) = delete;
            
            // Allow moving
            /**
             * @brief Move constructor
             * @param other Registry to move from
             */
            Registry(Registry&& other) noexcept {
                pools = move(other.pools);
            }
            
            /**
             * @brief Move assignment
             * @param other Registry to move from
             * @return Reference to this
             */
            Registry& operator=(Registry&& other) noexcept {
                if (this != &other) {
                    pools = move(other.pools);
                }
                return *this;
            }

            /**
             * @brief Adds a component to the specified entity by its ID
             * 
             * This function is used to add a component with a list of arguments
             * required/needed for that specific component. The component is stored
             * in an efficient pool with contiguous memory layout.
             * 
             * @tparam T Component Type (e.g., TransformComponent, HealthComponent)
             * @tparam Args Argument types for the component constructor
             * @param id Entity's ID to add the component to
             * @param args Arguments forwarded to component constructor
             * @return Reference to the newly added component
             * 
             * @note Use AddTagToEntity for tags instead of this function
             * @see AddTagToEntity
             * 
             * @example
             * @code
             * registry.Add<TransformComponent>(entityId, Vec2{100, 200});
             * @endcode
             */
            template<typename T, typename... Args>
            T& Add(EntityID id, Args&&... args) {
                if(pools.size() >= MAX_COMPONENT_QUANTITY) {
                    Debug::Logger::LogErrorWithType(ErrorType::EntityLimitReached, "Max component quantity reached!");
                    static T dummy{};
                    return dummy;
                }
                
                // Get or create the pool for this component type
                unique_ptr<IComponentPool>& pool = pools[typeid(T)];
                
                if (!pool) {
                    // Create new pool based on whether it's a tag or regular component
                    if constexpr (IsTag<T>) {
                        pool = make_unique<TagPool>();
                    } else {
                        pool = make_unique<ComponentPool<T>>();
                    }
                }
                
                if constexpr (IsTag<T>) {
                    auto* tagPool = static_cast<TagPool*>(pool.get());
                    tagPool->Set(id);
                } else {
                    // Regular components - store in efficient pool
                    auto* componentPool = static_cast<ComponentPool<T>*>(pool.get());
                    componentPool->Set(id, T{forward<Args>(args)...});
                }
                
                return Get<T>(id);
            }

            /**
             * @brief Add component with move semantics
             * 
             * More efficient version when you have an existing component
             * that can be moved rather than copied.
             * 
             * @tparam T Component type
             * @param id Entity ID
             * @param component Component to move into the pool
             * @return Reference to the stored component
             */
            template<typename T>
            T& Add(EntityID id, T&& component) {
                if(pools.size() >= MAX_COMPONENT_QUANTITY) {
                    Debug::Logger::LogErrorWithType(ErrorType::EntityLimitReached, "Max component quantity reached!");
                    static T dummy{};
                    return dummy;
                }
                
                unique_ptr<IComponentPool>& pool = pools[typeid(T)];
                
                if (!pool) {
                    if constexpr (IsTag<T>) {
                        pool = make_unique<TagPool>();
                    } else {
                        pool = make_unique<ComponentPool<T>>();
                    }
                }
                
                if constexpr (IsTag<T>) {
                    auto* tagPool = static_cast<TagPool*>(pool.get());
                    tagPool->Set(id);
                } else {
                    auto* componentPool = static_cast<ComponentPool<T>*>(pool.get());
                    componentPool->Set(id, move(component));
                }
                
                return Get<T>(id);
            }

            /**
             * @brief Get a component from the specified entity
             * 
             * Retrieves a reference to a component belonging to an entity.
             * 
             * @tparam T Component Type to retrieve
             * @param id Entity's ID
             * @return Reference to the component
             * 
             * @throws EntityException if component doesn't exist
             * 
             * @note Use Has<T>() to check if entity has the component first
             * @see Has
             */
            template<typename T>
            T& Get(EntityID id) {
                auto it = pools.find(typeid(T));
                if (it == pools.end()) {
                    Debug::Logger::LogErrorWithType(ErrorType::InvalidComponent, 
                        "Component type not found for entity!");
                    static T dummy{};
                    return dummy;
                }
                
                if constexpr (IsTag<T>) {
                    static T dummy{};
                    return dummy;
                } else {
                    auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
                    T* component = pool->Get(id);
                    if (!component) {
                        Debug::Logger::LogErrorWithType(ErrorType::InvalidComponent,
                            "Entity does not have this component!");
                        static T dummy{};
                        return dummy;
                    }
                    return *component;
                }
            }

            /**
             * @brief Get a const reference to a component
             * @tparam T Component type
             * @param id Entity ID
             * @return Const reference to component
             */
            template<typename T>
            const T& Get(EntityID id) const {
                auto it = pools.find(typeid(T));
                if (it == pools.end()) {
                    Debug::Logger::LogErrorWithType(ErrorType::InvalidComponent,
                        "Component type not found for entity!");
                    static T dummy{};
                    return dummy;
                }
                
                if constexpr (IsTag<T>) {
                    static T dummy{};
                    return dummy;
                } else {
                    auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
                    const T* component = pool->Get(id);
                    if (!component) {
                        Debug::Logger::LogErrorWithType(ErrorType::InvalidComponent,
                            "Entity does not have this component!");
                        static T dummy{};
                        return dummy;
                    }
                    return *component;
                }
            }

            /**
             * @brief Check if entity has a specific component
             * 
             * Efficiently checks if an entity has a specific component type
             * without throwing exceptions.
             * 
             * @tparam T Component Type to check
             * @param id Entity's ID to check
             * @return true if the component exists, false otherwise
             * 
             * @example
             * @code
             * if (registry.Has<TransformComponent>(entityId)) {
             *     auto& transform = registry.Get<TransformComponent>(entityId);
             * }
             * @endcode
             */
            template<typename T>
            bool Has(EntityID id) const {
                auto it = pools.find(typeid(T));
                if (it == pools.end())
                    return false;

                return it->second->Contains(id);
            }

            /**
             * @brief Check if entity has component by type_index
             * 
             * Runtime check using type_index, useful for serialization
             * and reflection-like functionality.
             * 
             * @param id Entity ID
             * @param type Component type to check
             * @return true if component exists
             */
            bool HasByType(EntityID id, const type_index& type) const {
                auto it = pools.find(type);
                if (it == pools.end())
                    return false;
                    
                return it->second->Contains(id);
            }

            /**
             * @brief Remove a component from entity
             * 
             * Removes a specific component type from an entity.
             * Uses efficient swap-and-pop removal.
             * 
             * @tparam T Component Type to remove
             * @param id Entity's ID
             * 
             * @note Does nothing if component doesn't exist
             */
            template<typename T>
            void Remove(EntityID id) {
                auto it = pools.find(typeid(T));
                if (it != pools.end()) {
                    it->second->Remove(id);
                    
                    // Clean up empty pools to save memory
                    if (it->second->Size() == 0) {
                        pools.erase(it);
                    }
                }
            }

            /**
             * @brief Get all component pools
             * 
             * Returns reference to the internal pool storage for
             * iteration and low-level operations.
             * 
             * @return Map of type_index to component pool
             */
            unordered_map<type_index, unique_ptr<IComponentPool>>& GetComponentPools() {
                return pools;
            }

            /**
             * @brief Get component types for an entity
             * 
             * Returns a list of all component type_indices that
             * an entity has. Useful for iteration and debugging.
             * 
             * @param id Entity ID
             * @return Vector of type_index for each component
             */
            vector<type_index> GetComponentsFromEntity(EntityID id) {
                vector<type_index> components;

                for (const auto& [type, pool] : pools) {
                    if (pool->Contains(id)) {
                        components.emplace_back(type);
                    }
                }

                return components;
            }

            /**
             * @brief Iterate over all components of an entity
             * 
             * Callback-based iteration over all components an entity has.
             * Primarily used for serialization.
             * 
             * @param id Entity ID to iterate over
             * @param func Function to call for each component
             * 
             * @note Tags are skipped in iteration
             */
            void ForEachComponentDo(EntityID id,
                                    const function<void(const ComponentTypeInfo&, const any&)> func) {
                for (const auto& [type, pool] : pools) {
                    if (!pool->Contains(id))
                        continue;

                    // Get component type info
                    const ComponentTypeInfo& info = ComponentRegistry::Get(type);
                    
                    // Skip tags
                    if (info.isTag)
                        continue;
                    
                    // Call with empty any - serialization now uses pools directly
                    // This is kept for backwards compatibility
                    any empty;
                    func(info, empty);
                }
            }
            
            /**
             * @brief Iterate over all registered component types
             * @param fn Function to call for each registered component type
             */
            void ForEachRegisteredComponent(const function<void(const ComponentTypeInfo&)>& fn) const {
                for (const auto& [type, info] : ComponentRegistry::GetComponentTypes()) {
                    fn(info);
                }
            }

            /**
             * @brief Get total size of all components for an entity
             * 
             * Calculates the total memory size needed to serialize
             * all components of an entity.
             * 
             * @param id Entity ID
             * @return Total size in bytes
             */
            size_t GetAllComponentsSize(EntityID id) {
                size_t size = 0;

                for (const auto& [type, pool] : pools) {
                    if (pool->Contains(id)) {
                        size += pool->GetComponentSize();
                    }
                }
                return size;
            }

            /**
             * @brief Check if type is a specific component type
             * @tparam T Component type to check against
             * @param type type_index to check
             * @return true if type matches T
             */
            template<typename T>
            bool IsComponentType(const type_index& type) {
                return type == type_index(typeid(T));
            }
            
            /**
             * @brief Clear all components for an entity
             * 
             * Removes all components from an entity efficiently.
             * Also cleans up any empty pools after removal.
             * 
             * @param id Entity ID to clear all components from
             */
            void RemoveAllComponents(EntityID id) {
                vector<type_index> componentTypes = GetComponentsFromEntity(id);
                for (const auto& type : componentTypes) {
                    auto it = pools.find(type);
                    if (it != pools.end()) {
                        it->second->Remove(id);
                    }
                }
                
                // Clean up empty pools
                for (auto it = pools.begin(); it != pools.end(); ) {
                    if (it->second->Size() == 0) {
                        it = pools.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

        private:
            /**
             * @brief Storage for all component pools
            */
            unordered_map<type_index, unique_ptr<IComponentPool>> pools;
    };
}
