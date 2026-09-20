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

namespace Galaktic::ECS { 
    class IComponentPool; 
} 

namespace Galaktic::ECS {

    /**
     * @class IComponentPool
     * @brief Abstract interface for component pools
     * 
     * This interface provides type-erased access to component pools, allowing
     * iteration over all component types and cleanup operations without knowing
     * the specific component type at compile time.
     * 
     * @note This is primarily used by the Registry class for managing pools
     *       and by serialization/deserialization code
     */
    class IComponentPool {
        public:
            /**
             * @brief Virtual destructor for proper cleanup
             */
            virtual ~IComponentPool() = default;
            
            /**
             * @brief Remove a component for a specific entity
             * @param id Entity's ID whose component should be removed
             */
            virtual void Remove(EntityID id) = 0;
            
            /**
             * @brief Check if an entity has a component in this pool
             * @param id Entity's ID to check
             * @return true if the entity has a component in this pool
             */
            virtual bool Contains(EntityID id) const = 0;
            
            /**
             * @brief Get the number of components in this pool
             * @return Number of components stored
             */
            virtual size_t Size() const = 0;
            
            /**
             * @brief Clear all components from this pool
             */
            virtual void Clear() = 0;
            
            /**
             * @brief Get the size of the component type stored in this pool
             * @return Size in bytes of a single component
             */
            virtual size_t GetComponentSize() const = 0;
            
            /**
             * @brief Serialize a specific component to file
             * @param id Entity's ID whose component should be serialized
             * @param file Output file stream
             * 
             * @note For POD types, uses binary write. For non-POD types,
             *       calls the component's Write method
             */
            virtual void Serialize(EntityID id, std::ofstream& file) const = 0;
            
            /**
             * @brief Deserialize and add a component from file
             * @param id Entity's ID to assign the deserialized component to
             * @param file file stream
             * 
             * @note For POD types, uses binary read. For non-POD types,
             *       calls the component's Read method
             */
            virtual void Deserialize(EntityID id, std::ifstream& file) = 0;
    };

    /**
     * @class ComponentPool
     * @brief Efficient templated component pool using contiguous memory
     * 
     * @tparam T Component type to store
     * 
     * @example
     * @code
     * ComponentPool<TransformComponent> pool;
     * pool.Set(entityId, TransformComponent{...});
     * TransformComponent* comp = pool.Get(entityId);
     * pool.Remove(entityId);
     * @endcode
     */
    template<typename T>
    class ComponentPool : public IComponentPool {
        public:
            /**
             * @brief Default constructor
             */
            ComponentPool() = default;
            
            /**
             * @brief Add or update a component for an entity
             * @param id Entity's ID
             * @param component Component to add (copied)
             * 
             * If the entity already has a component, it will be replaced.
             * Uses copy semantics for the component data.
             */
            void Set(EntityID id, const T& component) {
                auto it = m_entityToIndex.find(id);
                if (it != m_entityToIndex.end()) {
                    // Update existing component
                    m_components[it->second] = component;
                } else {
                    // Add new component
                    size_t newIndex = m_components.size();
                    m_components.push_back(component);
                    m_entityToIndex[id] = newIndex;
                    m_indexToEntity[newIndex] = id;
                }
            }
            
            /**
             * @brief Add or update a component using move semantics
             * @param id Entity's ID
             * @param component Component to add (will be moved)
             * 
             * More efficient than Set() when the component is a temporary
             * or can be moved from.
             */
            void Set(EntityID id, T&& component) {
                auto it = m_entityToIndex.find(id);
                if (it != m_entityToIndex.end()) {
                    // Update existing component
                    m_components[it->second] = std::move(component);
                } else {
                    // Add new component
                    size_t newIndex = m_components.size();
                    m_components.push_back(std::move(component));
                    m_entityToIndex[id] = newIndex;
                    m_indexToEntity[newIndex] = id;
                }
            }
            
            /**
             * @brief Get a pointer to a component for an entity
             * @param id Entity's ID
             * @return Pointer to component, nullptr if not found
             * 
             * @note Returns nullptr if entity doesn't have this component,
             *       so callers should check before dereferencing
             */
            T* Get(EntityID id) {
                auto it = m_entityToIndex.find(id);
                if (it != m_entityToIndex.end()) {
                    return &m_components[it->second];
                }
                return nullptr;
            }
            
            /**
             * @brief Get a const pointer to a component
             * @param id Entity's ID
             * @return Const pointer to component, nullptr if not found
             */
            const T* Get(EntityID id) const {
                auto it = m_entityToIndex.find(id);
                if (it != m_entityToIndex.end()) {
                    return &m_components[it->second];
                }
                return nullptr;
            }
            
            /**
             * @brief Check if entity has this component
             * @param id Entity's ID
             * @return true if component exists
             */
            bool Contains(EntityID id) const override {
                return m_entityToIndex.find(id) != m_entityToIndex.end();
            }
            
            /**
             * @brief Remove component from entity using swap-and-pop
             * @param id Entity's ID
             */
            void Remove(EntityID id) override {
                auto it = m_entityToIndex.find(id);
                if (it != m_entityToIndex.end()) {
                    size_t indexToRemove = it->second;
                    size_t lastIndex = m_components.size() - 1;
                    
                    if (indexToRemove != lastIndex) {
                        // Move last element to removed position
                        m_components[indexToRemove] = std::move(m_components[lastIndex]);
                        
                        // Update the Entity's ID that was at the last position
                        EntityID lastEntityId = m_indexToEntity[lastIndex];
                        m_entityToIndex[lastEntityId] = indexToRemove;
                        m_indexToEntity[indexToRemove] = lastEntityId;
                    }
                    
                    // Remove from maps
                    m_entityToIndex.erase(it);
                    m_indexToEntity.erase(lastIndex);
                    m_components.pop_back();
                }
            }
            
            /**
             * @brief Get number of components in pool
             * @return Number of components
             */
            size_t Size() const override {
                return m_components.size();
            }
            
            /**
             * @brief Clear all components
             * 
             * Removes all components from the pool and clears
             * all internal data structures.
             */
            void Clear() override {
                m_components.clear();
                m_entityToIndex.clear();
                m_indexToEntity.clear();
            }
            
            /**
             * @brief Get size of component type
             * @return Size in bytes
             */
            size_t GetComponentSize() const override {
                return sizeof(T);
            }
            
            /**
             * @brief Serialize component to binary file
             * @param id Entity's ID
             * @param file Output file stream
             * 
             * For trivially copyable types (POD), uses binary write.
             * For non-POD types, calls Component::Write()
             */
            void Serialize(EntityID id, ofstream& file) const override {
                const T* comp = Get(id);
                if (comp) {
                    if constexpr (std::is_trivially_copyable_v<T>) {
                        file.write(GKC_WRITE_BINARY(*comp), sizeof(T));
                    } else {
                        T::Write(file, *comp);
                    }
                }
            }
            
            /**
             * @brief Deserialize component from binary file
             * @param id Entity's ID
             * @param file file stream
             * 
             * For trivially copyable types (POD), uses binary read.
             * For non-POD types, calls Component::Read()
             */
            void Deserialize(EntityID id, ifstream& file) override {
                T component{};
                if constexpr (std::is_trivially_copyable_v<T>) {
                    file.read(GKC_READ_BINARY(component), sizeof(T));
                } else {
                    T::Read(file, component);
                }
                Set(id, std::move(component));
            }
            
            /**
             * @brief Get or create component for entity
             * @param id Entity's ID
             * @return Reference to the component (existing or newly created)
             * 
             * If the entity doesn't have a component, creates a default-initialized
             * one. Useful when you need a component to exist.
             */
            T& GetOrCreate(EntityID id) {
                auto it = m_entityToIndex.find(id);
                if (it != m_entityToIndex.end()) {
                    return m_components[it->second];
                }
                size_t newIndex = m_components.size();
                m_components.push_back(T{});
                m_entityToIndex[id] = newIndex;
                m_indexToEntity[newIndex] = id;
                return m_components.back();
            }
            
            // Iterator support for range-based for loops
            auto begin() { return m_components.begin(); }
            auto end() { return m_components.end(); }
            auto begin() const { return m_components.begin(); }
            auto end() const { return m_components.end(); }
            
        private:
            vector<T> m_components;     
            unordered_map<EntityID, size_t> m_entityToIndex;
            unordered_map<size_t, EntityID> m_indexToEntity;
    };

    /**
     * @class TagPool
     * @brief Specialized pool for tag components (empty marker types)
     * 
     * Tags are marker components with no data - they just mark that an entity
     * belongs to a certain category. This pool efficiently stores just the
     * set of Entity's IDs that have each tag.
     * 
     * @example
     * @code
     * TagPool tagPool;
     * tagPool.Set(entityId);  // Mark entity as player
     * bool isPlayer = tagPool.Contains(entityId);
     * tagPool.Remove(entityId);  // Remove tag
     * @endcode
     */
    class TagPool : public IComponentPool {
        public:
            /**
             * @brief Mark an entity as having this tag
             * @param id Entity's ID to tag
             */
            void Set(EntityID id) {
                m_entities.insert(id);
            }
            
            /**
             * @brief Remove tag from entity
             * @param id Entity's ID
             */
            void Remove(EntityID id) override {
                m_entities.erase(id);
            }
            
            /**
             * @brief Check if entity has this tag
             * @param id Entity's ID
             * @return true if entity is tagged
             */
            bool Contains(EntityID id) const override {
                return m_entities.find(id) != m_entities.end();
            }
            
            /**
             * @brief Get number of tagged entities
             * @return Count
             */
            size_t Size() const override {
                return m_entities.size();
            }
            
            /**
             * @brief Clear all tags
             */
            void Clear() override {
                m_entities.clear();
            }
            
            /**
             * @brief Tags have zero size
             * @return 0
             */
            size_t GetComponentSize() const override {
                return 0;
            }
            
            /**
             * @brief Tags have no data to serialize
             * @param id Entity's ID
             * @param file file
             */
            void Serialize(EntityID id, ofstream& file) const override {
                // tags are just markers xd
            }
            
            /**
             * @brief Deserialize tag
             * @param id Entity's ID
             * @param file file
             */
            void Deserialize(EntityID id, ifstream& file) override {
                Set(id);
            }
            
        private:
            std::unordered_set<EntityID> m_entities;
    };

} // namespace Galaktic::ECS
