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
#include <ecs/gkc_registry.h>
#include "ecs/gkc_entity.h"
#include <ecs/gkc_components.h>
#include "core/gkc_logger.h"
#include "core/systems/gkc_key.h"
#include "ecs/gkc_template_traits.h"

namespace Galaktic::ECS {
    /**
     * Checks if a component exists inside an entity, if it does executes
     * a custom void function
     * @tparam T Component Type
     * @param entity Entity
     * @param func function to execute
     */
    template <typename T>
    inline void IfComponentExists(ECS::Entity& entity, function<void()> func) {
        if (entity.Has<T>()) {
            func();
        }
    }
}
namespace Galaktic::Core::Managers {

    // I thought this class was going to be small but hell naw man, this thing
    // gets bigger every time I add entity features :,D

    /**
     * @class ECS_Manager
     * @brief Manages all entities inside a scene
     *
     * Manager for all entities inside a scene, this manager is an intermediate bridge between
     * the \c ECS_Helper class and the \c Registry, this class manages things like
     * components, ID's and associated names with entities, (using a \c std::multimap).
     * When entities are added their IDs are assigned one by one, <b> (e.g. the third entity added
     * will have an ID of 3, and so on) </b>
     *
     * The components are also registered in another class called \c ComponentRegistry which
     * is used for saving an entity's components when a scene is saved into a file (.gkscene format)
     * @see gkc_component_registry.h for more information
     *
     * @attention Each function inside this class is written to help with one or many specific things,
     * their behaviour and what they specifically do is documented, please <b>READ THEM</b>
     * before using them to do anything with an entity or its components :)
     */
    class ECS_Manager {
        public:
            /**
             * @param registry Registry pointer
             */
            explicit ECS_Manager(ECS::Registry* registry) : m_registry(registry) {
                GKC_RELEASE_ASSERT(registry != nullptr, "ECS_Manager registry is NULL, aborting...");
            }

            /**
             * @brief Adds a component to the desired entity by ID
             *
             * This function is used to add a component with a list of arguments
             * required/needed for that specific component, use \c AddTagToEntity
             * if you want to add tags
             * @tparam T Component type
             * @tparam Args Argument types
             * @param id Entity's ID
             * @param args Arguments of the component
             */
            template<typename T, typename... Args>
            void AddComponentToEntity(EntityID id, Args&& ... args) {
                GKC_RELEASE_ASSERT(!ECS::IsTag<T>, "use AddTagToEntity for tags!");
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;
                m_registry->Add<T>(id, std::forward<Args>(args)...);
                ECS::ComponentRegistry::RegisterComponent<T>(id, false);
            }

            /**
             * @brief Adds a tag component to the desired entity by ID
             *
             * This function is <b>ONLY</b> used for adding tags to entities,
             * use \c AddComponentToEntity for components
             * @tparam T Tag type
             * @param id Entity's ID
             */
            template<typename T>
            void AddTagToEntity(EntityID id) {
                GKC_RELEASE_ASSERT(ECS::IsTag<T>, "use AddComponentToEntity for components!");
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;
                m_registry->Add<T>(id);
                ECS::ComponentRegistry::RegisterComponent<T>(id, true);
            }

            /**
             * @brief Adds a raw component to an entity by specifying its type
             *
             * Adds a raw component into the registry component pools,
             * this function is used when a component needs to be added directly
             * to the pool without any arguments or template parameters for
             * that component
             * @param id Entity's ID
             * @param type Component's Type
             * @param comp Component
             */
            void AddRawComponentToEntity(EntityID id, const type_index& type,
                                         any&& comp) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;

                m_registry->GetComponentPools()[type][id] = std::move(comp);
                if (!ECS::ComponentRegistry::IsRegistered(type)) {
                    ECS::ComponentRegistry::RegisterComponentByType(type, id, false);
                }
            }

            void DeleteRawComponentFromEntity(EntityID id, const type_index& type) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;

                auto& componentPool = m_registry->GetComponentPools()[type];
                if(ECS::ComponentRegistry::IsRegistered(type) && componentPool.contains(id)) {
                    ECS::ComponentRegistry::UnregisterComponentByType(type);
                    componentPool.erase(id);
                }
            }

            /**
             * @brief Adds a raw tag to an entity by specifying its type
             *
             * Adds a raw tag component into the registry component pools,
             * this function is used when a tag needs to be added directly
             * to the pool without a template parameter
             * @param id
             * @param type
             */
            void AddTagByType(EntityID id, const type_index& type) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;

                m_registry->GetComponentPools()[type][id] = any{};
                if (!ECS::ComponentRegistry::IsRegistered(type)) {
                    ECS::ComponentRegistry::RegisterComponentByType(type, id, false);
                }
            }

            /**
             * @brief Creates an entity with a name component and a tag
             *
             * Creates an entity adding automatically a name component and
             * a tag using a template parameter.
             * @tparam T Tag Component
             * @param name Name of the entity
             */
            template<typename T>
            ECS::Entity CreateEntity(const string& name) {
                EntityID id = m_entityList.size() + 1;
                ECS::Entity entity = ECS::Entity(id, m_registry);
                m_entityList.emplace(id, entity);
                string uniqueName = Core::GenerateUniqueName(m_nameToEntityList, name);
                AddComponentToEntity<ECS::NameComponent>(id, uniqueName);
                AddTagToEntity<T>(id);
                m_nameToEntityList.emplace(uniqueName, id);
                return entity;
            }

            /**
             * @brief Creates an entity with a name component and a tag specified
             * by type
             *
             * Creates an entity adding automatically a name component and
             * a tag by specifying its type
             * @param name Name of the entity
             * @param type Tag type
             */
            void CreateEntityByTypeIndex(const string& name, const type_index& type) {
                EntityID id = m_entityList.size() + 1;
                ECS::Entity entity = ECS::Entity(id, m_registry);
                m_entityList.emplace(id, entity);

                string uniqueName = Core::GenerateUniqueName(m_nameToEntityList, name);
                AddComponentToEntity<ECS::NameComponent>(id, uniqueName);
                AddTagByType(id, type);
                m_nameToEntityList.emplace(uniqueName, id);
            }

            /**
             * @brief Adds an empty entity to the entity list
             *
             * This function is used (for now) to register an entity
             * before reading its components from a .gkscene file,
             * when reading components an entity <i>should</i> already
             * exist in the entity list to add the components while
             * reading the file, that's why this thing exists :P
             *
             * @param id Entity's ID
             * @param entity Entity
             */
            void AddEmptyEntity(EntityID id, ECS::Entity& entity) {
                m_entityList.emplace(id, entity);
            }

            /**
             * @brief Replaces the entity that has the passed ID, if the entity
             * exists its replaced by the passed entity
             * @param id Entity's ID
             * @param entity Entity
             */
            void ReplaceEntityByID(EntityID id, ECS::Entity& entity) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;

                it->second = entity;
            }

            /**
             * @brief Gets a reference to the component of an entity by ID
             * @tparam T Component type
             * @param id Entity's ID
             * @return Returns a reference to the component's Entity
             */
            template<typename T>
            T& GetComponentOfEntity(EntityID id) {
                return m_registry->Get<T>(id);
            }

            /**
             * Renames the entity using the entity's ID if it has a NameComponent
             * component, both the key and the actual name of the entity are replaced
             * @param id Entity's ID
             * @param newName New name of the entity
             */
            void RenameEntity(EntityID id, const string& newName) {
                if (!m_entityList.at(id).Has<ECS::NameComponent>())
                    return;

                ECS::NameComponent& nameComp = m_entityList[id].Get<ECS::NameComponent>();
                string uniqueName = Core::GenerateUniqueName(m_nameToEntityList, newName);
                nameComp.m_name = uniqueName;
                m_nameToEntityList.insert_or_assign(nameComp.m_name, id);
            }
            /**
             * @brief Returns a pointer to the specified entity by name
             * @param name Entity's name
             * @return A pointer if the entity exists and nullptr otherwise
             * @note This function returns the first entity created with the same name,
             *       use \c GetEntitiesByName() for multiple entities with the same name
             */
            ECS::Entity* GetEntityByName(const string& name) {
                auto it = m_nameToEntityList.find(name);
                if (it != m_nameToEntityList.end()) {
                    return &m_entityList[it->second];
                }
                GKC_ENGINE_WARNING("Entity retrieved doesn't exist!");
                return nullptr;
            }
            
            /**
             * @brief Returns a pointer to the specified entity by ID
             * @param id Entity's ID
             * @return A pointer if the entity exists and nullptr otherwise
             */
            ECS::Entity* GetEntityByID(EntityID id) {
                auto it = m_entityList.find(id);
                if (it != m_entityList.end()) {
                    return &it->second;
                }
                GKC_ENGINE_WARNING("Entity retrieved doesn't exist!");
                return nullptr;
            }

            /**
             * @brief Prints a list of which entities are valid or invalid,
             *        if the ID is 0 that means the entity is invalid, valid otherwise
             * @param entities Vector of entity references
             */
            void PrintEntitiesIntegrity(const vector<ECS::Entity>& entities) {
                for (auto& entity : entities) {
                    bool isValid = entity.IsValid();
                    EntityID id = InvalidEntity;
                    if (isValid) {
                        id = entity.GetID();
                    }

                    // Helper to print "INVALID" or "VALID"
                    auto InvalidOrValidString = [&] {
                        switch (id) {
                            case InvalidEntity:
                                return "INVALID";
                            default:
                                return "VALID";
                        }
                    }();
                    GKC_ENGINE_INFO("Entity (ID: {0}) -> {1}", id, InvalidOrValidString);
                }
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

            string GetEntityNameByID(EntityID id) {
                auto it = m_entityList.find(id);
                if (it != m_entityList.end()) {
                    ECS::IfComponentExists<ECS::NameComponent>(it->second, [&]() {
                        auto& nameComp = it->second.Get<ECS::NameComponent>();
                        return nameComp.m_name;
                    });
                    return "UnknownName";
                }
                return "UnknownName";
            } 

            /**
             * @brief Deletes the entity by its ID
             * @param id Entity's ID
             */
            void DeleteEntity(EntityID id) {
                if(m_entityList.contains(id)) {
                    // CRITICAL FIX: Remove all components from the registry before deleting entity
                    // Get all component types and remove them
                    auto& componentPools = m_registry->GetComponentPools();
                    for(auto& [type, pool] : componentPools) {
                        if(pool.contains(id)) {
                            pool.erase(id);
                        }
                    }
                    // Also remove from name index
                    auto it = m_entityList.find(id);
                    if(it != m_entityList.end()) {
                        if(it->second.Has<ECS::NameComponent>()) {
                            auto& nameComp = it->second.Get<ECS::NameComponent>();
                            m_nameToEntityList.erase(nameComp.m_name);
                        }
                    }
                    m_entityList.erase(id);
                }
            }

            void DeleteEntityByName(const string& name) {
                auto it = m_nameToEntityList.find(name);
                if (it != m_nameToEntityList.end()) {
                    DeleteEntity(it->second);
                    m_nameToEntityList.erase(it);
                }
            }

            size_t GetEntityCount() {
                return m_entityList.size();
            }

            size_t GetComponentsCount() {
                return m_registry->GetComponentPools().size();
            }

            ECS::Entity_List& GetEntityList() { return m_entityList; }
            ECS::NameToEntity_List& GetNameToEntityList() { return m_nameToEntityList; }
            ECS::Registry*& GetRegistry() { return m_registry; }
        private:
            ECS::Entity_List m_entityList;
            ECS::NameToEntity_List m_nameToEntityList;
            ECS::Registry* m_registry;
    };
}

