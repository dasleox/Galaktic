#include <core/helpers/gkc_ecs_helper.h>
#include "core/gkc_logger.h"
#include "core/managers/gkc_ecs_man.h"
#include "ecs/gkc_entity.h"

using namespace Galaktic::Core;
using namespace Galaktic;

EntityID Helpers::ECS_Helper::m_id = 0;
Galaktic::Core::Managers::ECS_Manager* Helpers::ECS_Helper::m_ecsManager = nullptr;

Helpers::ECS_Helper::ECS_Helper(Managers::ECS_Manager* ecsManager) {
    m_ecsManager = ecsManager;
}

ECS::Entity Helpers::ECS_Helper::CreatePlayer(const string &name) {
    ECS::Entity player = m_ecsManager->CreateEntity<ECS::PlayerTag>(name);
    ECS::TransformComponent transform;                          // 50x50.f size, X: 0, Y: 0, rotation: 0
    ECS::SpeedComponent speed;                                  // 1000.f max speed by default
    ECS::HealthComponent health;                                // 100.f max health, can take damage by default
    ECS::ColorComponent color(GREY_COLOR);                      // Grey color by default
    ECS::CollisionComponent collision(transform.m_size , true); // Collisions true by default
    ECS::JumpComponent jump;                                    // 100.f jump height, can jump by default
    ECS::RigidBody rigidBody;                                   

    m_ecsManager->AddComponentToEntity<ECS::TransformComponent>(player.GetID(), transform);
    m_ecsManager->AddComponentToEntity<ECS::SpeedComponent>(player.GetID(), speed);
    m_ecsManager->AddComponentToEntity<ECS::HealthComponent>(player.GetID(), health);
    m_ecsManager->AddComponentToEntity<ECS::ColorComponent>(player.GetID(), color);
    m_ecsManager->AddComponentToEntity<ECS::CollisionComponent>(player.GetID(), collision);
    m_ecsManager->AddComponentToEntity<ECS::JumpComponent>(player.GetID(), jump);
    m_ecsManager->AddComponentToEntity<ECS::RigidBody>(player.GetID(), rigidBody);
    return player;
}

ECS::Entity Helpers::ECS_Helper::CreateStaticObject(const string &name) {
    ECS::Entity staticObject = m_ecsManager->CreateEntity<ECS::StaticObjectTag>(name);
    ECS::TransformComponent transform;      
    ECS::ColorComponent color;              
    ECS::CollisionComponent collision(transform.m_size , true);        // Collision box same as size of the object, collidable by default
    ECS::VisibilityComponent visibility(true);                        // Visible by default

    m_ecsManager->AddComponentToEntity<ECS::TransformComponent>(staticObject.GetID(), transform);
    m_ecsManager->AddComponentToEntity<ECS::ColorComponent>(staticObject.GetID(), color);
    m_ecsManager->AddComponentToEntity<ECS::CollisionComponent>(staticObject.GetID(), collision);
    m_ecsManager->AddComponentToEntity<ECS::VisibilityComponent>(staticObject.GetID(), visibility);
    return staticObject;
}

ECS::Entity Helpers::ECS_Helper::CreatePhysicsObject(const string &name) {
    ECS::Entity physicsObject = m_ecsManager->CreateEntity<ECS::PhysicsObjectTag>(name);
    ECS::TransformComponent transform;     
    ECS::ColorComponent color;            
    ECS::CollisionComponent collision(transform.m_size, true); 
    ECS::RigidBody rigidBody;                                   // 0 velocity and force by default, mass of 1.f
    ECS::VisibilityComponent visibility;        

    m_ecsManager->AddComponentToEntity<ECS::TransformComponent>(physicsObject.GetID(), transform);
    m_ecsManager->AddComponentToEntity<ECS::ColorComponent>(physicsObject.GetID(), color);
    m_ecsManager->AddComponentToEntity<ECS::CollisionComponent>(physicsObject.GetID(), collision);
    m_ecsManager->AddComponentToEntity<ECS::RigidBody>(physicsObject.GetID(), rigidBody);
    m_ecsManager->AddComponentToEntity<ECS::VisibilityComponent>(physicsObject.GetID(), visibility);
    return physicsObject;
}

ECS::Entity Helpers::ECS_Helper::CreateLightEntity(const string &name) {
    ECS::Entity light = m_ecsManager->CreateEntity<ECS::LightTag>(name);
    ECS::LightComponent lightComponent;     // Default light component for light entity

    m_ecsManager->AddComponentToEntity<ECS::LightComponent>(light.GetID(), lightComponent);
    return light;
}

ECS::Entity Helpers::ECS_Helper::CreateCameraEntity(const string &name) {
    ECS::Entity camera = m_ecsManager->CreateEntity<ECS::CameraTag>(name);
    ECS::CameraComponent cameraComponent;   // Default camera component for camera entity

    m_ecsManager->AddComponentToEntity<ECS::CameraComponent>(camera.GetID(), cameraComponent);
    return camera;
}

void Helpers::ECS_Helper::DeleteEntity(const string &name) {
    m_ecsManager->DeleteEntityByName(name);
}

void Helpers::ECS_Helper::AddComponentToEntity(const string &name, const type_index &type, any& component) {
    auto it = m_ecsManager->GetNameToEntityList().find(name);
    if (it == m_ecsManager->GetNameToEntityList().end()) {
        GKC_ENGINE_ERROR("Entity with name '{}' does not exist!", name);
        return;
    }
    EntityID id = it->second;
    if(m_ecsManager->GetRegistry()->HasByType(id, type)) {
        GKC_ENGINE_INFO("Entity '{}' already has component of type '{}'", 
            name, Debug::Logger::DemangleTypename(type.name()));
        return;
    }

    if(HasNameComponent(type)) {
        GKC_ENGINE_ERROR("Entities already have a NameComponent '{}'", name);
        return;
    }

    if(!ValidateComponentCompatibility(name, type)) {
        return;
    }

    m_ecsManager->AddRawComponentToEntity(id, type, std::move(component));
}

void Helpers::ECS_Helper::RemoveComponentFromEntity(const string &name, const type_index &type) {
    auto it = m_ecsManager->GetNameToEntityList().find(name);
    if (it == m_ecsManager->GetNameToEntityList().end()) {
        GKC_ENGINE_ERROR("Entity with name '{}' does not exist!", name);
        return;
    }
    EntityID id = it->second;
    m_ecsManager->DeleteRawComponentFromEntity(id, type);
}

ECS::Entity& Helpers::ECS_Helper::GetEntityByName(const string &name) {
    return *m_ecsManager->GetEntityByName(name);
}

// Checkers

bool Helpers::ECS_Helper::ValidateComponentCompatibility(const string& name, const type_index& type) {
    // Return false when a component is not compatible with the entity's type, otherwise return true
    if(IsLightEntity(name) && !IsLightComponent(type)) {
        GKC_ENGINE_ERROR("Attempted to add a non-compatible component to a Light entity '{}'", name);
        return false;
    }
    else if (IsCameraEntity(name) && !IsCameraComponent(type)) {
        GKC_ENGINE_ERROR("Attempted to add a non-compatible component to a Camera entity '{}'", name);
        return false;
    }
    else if (IsPlayerEntity(name) && !IsPlayerComponent(type)) {
        GKC_ENGINE_ERROR("Attempted to add a non-compatible component to a Player entity '{}'", name);
        return false;
    } else if (IsStaticObjectEntity(name) && !IsStaticObjectComponent(type)) {
        GKC_ENGINE_ERROR("Attempted to add a non-compatible component to a Static Object entity '{}'", name);
        return false;
    } else if (IsPhysicsObjectEntity(name) && !IsPhysicsObjectComponent(type)) {
        GKC_ENGINE_ERROR("Attempted to add a non-compatible component to a Physics Object entity '{}'", name);
        return false;
    }
    return true; // If component is compatible, return true
}

bool Helpers::ECS_Helper::IsLightEntity(const string &name) {
    auto entity = m_ecsManager->GetEntityByName(name);
    return entity->Has<ECS::LightTag>();
}

bool Helpers::ECS_Helper::IsCameraEntity(const string &name) {
    auto entity = m_ecsManager->GetEntityByName(name);
    return entity->Has<ECS::CameraTag>();
}

bool Helpers::ECS_Helper::IsPlayerEntity(const string &name) {
    auto entity = m_ecsManager->GetEntityByName(name);
    return entity->Has<ECS::PlayerTag>();
}

bool Helpers::ECS_Helper::IsStaticObjectEntity(const string &name) {
    auto entity = m_ecsManager->GetEntityByName(name);
    return entity->Has<ECS::StaticObjectTag>();
}

bool Helpers::ECS_Helper::IsPhysicsObjectEntity(const string &name) {
    auto entity = m_ecsManager->GetEntityByName(name);
    return entity->Has<ECS::PhysicsObjectTag>();
}

bool Helpers::ECS_Helper::HasNameComponent(const type_index &type) {
    return type == typeid(ECS::NameComponent);
}

bool Helpers::ECS_Helper::IsLightComponent(const type_index& type) {
    return type == typeid(ECS::LightComponent) || type == typeid(ECS::LightTag);
}

bool Helpers::ECS_Helper::IsCameraComponent(const type_index& type) {
    return type == typeid(ECS::CameraComponent) || type == typeid(ECS::CameraTag);
}

bool Helpers::ECS_Helper::IsPlayerComponent(const type_index& type) {
    return type == typeid(ECS::PlayerTag) || 
        type == typeid(ECS::TransformComponent) ||
        type == typeid(ECS::SpeedComponent) ||
        type == typeid(ECS::HealthComponent) ||
        type == typeid(ECS::ColorComponent) ||
        type == typeid(ECS::CollisionComponent) ||
        type == typeid(ECS::JumpComponent) ||
        type == typeid(ECS::RigidBody);
}

bool Helpers::ECS_Helper::IsStaticObjectComponent(const type_index& type) {
    return type == typeid(ECS::StaticObjectTag) ||
        type == typeid(ECS::TransformComponent) ||
        type == typeid(ECS::ColorComponent) ||
        type == typeid(ECS::CollisionComponent) ||
        type == typeid(ECS::HealthComponent);
}

bool Helpers::ECS_Helper::IsPhysicsObjectComponent(const type_index& type) {
    return type == typeid(ECS::PhysicsObjectTag) ||
        type == typeid(ECS::TransformComponent) ||
        type == typeid(ECS::ColorComponent) ||
        type == typeid(ECS::CollisionComponent) ||
        type == typeid(ECS::RigidBody) ||
        type == typeid(ECS::HealthComponent);
}