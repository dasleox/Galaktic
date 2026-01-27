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
#include "core/gkc_logger.h"

namespace Galaktic::Core::Managers {

    template<typename T>
    constexpr bool IsTag = false;
    template<> constexpr bool IsTag<ECS::PlayerTag> = true;
    template<> constexpr bool IsTag<ECS::StaticObjectTag> = true;
    template<> constexpr bool IsTag<ECS::PhysicsObjectTag> = true;
    template<> constexpr bool IsTag<ECS::CameraTag> = true;
    template<> constexpr bool IsTag<ECS::LightTag> = true;
    template<> constexpr bool IsTag<ECS::EnemyTag> = true;

    /**
     * @class ECS_Manager
     * @brief A bridge between the entity registry and the scene
     */
    class ECS_Manager {
        public:
            explicit ECS_Manager(ECS::Registry* registry) : m_registry(registry) {}

            /**
             * @brief Adds a component to the desired entity by ID
             * @tparam T Component type
             * @tparam Args Argument types
             * @param id Entity's ID
             * @param args Arguments of the component
             */
            template<typename T, typename... Args>
            void AddComponentToEntity(EntityID id, Args&& ... args) {
                GKC_ASSERT(!IsTag<T>, "use AddTagToEntity for tags!");
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;
                m_registry->Add<T>(id, std::forward<Args>(args)...);
                ECS::ComponentRegistry::RegisterComponent<T>(id, false);
            }

            template<typename T>
            void AddTagToEntity(EntityID id) {
                GKC_ASSERT(IsTag<T>, "use AddComponentToEntity for components!");
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;
                m_registry->Add<T>(id);
                ECS::ComponentRegistry::RegisterComponent<T>(id, true);
            }

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
             * @tparam T Tag Component
             * @param name Name of the entity
             * @note This function already adds a name component
             */
            template<typename T>
            void CreateEntity(const string& name) {
                // Entity ID's increased by the size of the list
                EntityID id = m_entityList.size() + 1;
                ECS::Entity entity = ECS::Entity(id, m_registry);
                m_entityList.emplace(id, entity);

                // Add a name component to the entity
                AddComponentToEntity<ECS::NameComponent>(id, name);

                // Adds a Tag for the entity
                AddTagToEntity<T>(id);
                m_nameToEntityList.emplace(name, id);
            }

            void CreateEmptyEntity(EntityID id, ECS::Entity& entity) {
                m_entityList.emplace(id, entity);
            }

            void ReplaceEntityByID(EntityID id, ECS::Entity& entity) {
                auto it = m_entityList.find(id);
                if (it == m_entityList.end())
                    return;

                it->second = entity;
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
             * @brief Renames the entity
             * @param id Entity's ID
             * @param newName New name of the entity
             */
            void RenameEntity(EntityID id, const string& newName) {
                auto& nameComp = m_entityList[id].Get<ECS::NameComponent>();
                m_nameToEntityList.erase(nameComp.name_);

                nameComp.name_ = newName;
                m_nameToEntityList.emplace(newName, id);
            }

            /**
             * @brief Returns a pointer to the specified entity by name
             * @param name Entity's name
             * @return A pointer if the entity exists and nullptr otherwise
             */
            ECS::Entity* GetEntityByName(const string& name) {
                auto it = m_nameToEntityList.find(name);
                if (it == m_nameToEntityList.end())
                    return nullptr;
                return &m_entityList[it->second];
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
            ECS::NameToEntity_List& GetNameToEntityList() { return m_nameToEntityList; }
            ECS::Registry*& GetRegistry() { return m_registry; }
        private:
            ECS::Entity_List m_entityList;
            ECS::NameToEntity_List m_nameToEntityList;
            ECS::Registry* m_registry;
    };
}
