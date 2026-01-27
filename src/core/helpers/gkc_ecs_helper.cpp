#include <core/helpers/gkc_ecs_helper.h>
#include "core/gkc_logger.h"
#include "core/managers/gkc_ecs_man.h"

using namespace Galaktic::Core;
Helpers::ECS_Helper::ECS_Helper(Managers::ECS_Manager &ecsManager)
    : m_ecsManager(ecsManager) {}


void Helpers::ECS_Helper::CreatePlayer(const string &name) {
    ECS::TransformComponent transform; transform.size_ = {50.f, 50.f};
    ECS::SpeedComponent speed;
    ECS::ColorComponent color;
    m_id++;

    m_ecsManager.CreateEntity<ECS::PlayerTag>(name);
    m_ecsManager.AddComponentToEntity<ECS::TransformComponent>(m_id, transform);
    m_ecsManager.AddComponentToEntity<ECS::SpeedComponent>(m_id, speed);
    m_ecsManager.AddComponentToEntity<ECS::ColorComponent>(m_id, color);
}

void Helpers::ECS_Helper::CreateStaticObject(const string &name) {
    ECS::TransformComponent transform; transform.size_ = {50.f, 50.f};
    ECS::ColorComponent color;
    m_id++;

    m_ecsManager.CreateEntity<ECS::StaticObjectTag>(name);
    m_ecsManager.AddComponentToEntity<ECS::TransformComponent>(m_id, transform);
    m_ecsManager.AddComponentToEntity<ECS::ColorComponent>(m_id, color);
}

void Helpers::ECS_Helper::CreatePhysicsObject(const string &name) {
    ECS::TransformComponent transform; transform.size_ = {50.f, 50.f};
    ECS::ColorComponent color;
    m_id++;

    m_ecsManager.CreateEntity<ECS::PhysicsObjectTag>(name);
    m_ecsManager.AddComponentToEntity<ECS::TransformComponent>(m_id, transform);
    m_ecsManager.AddComponentToEntity<ECS::ColorComponent>(m_id, color);
}

void Helpers::ECS_Helper::CreateLightEntity(const string &name) {
    m_id++;
    m_ecsManager.CreateEntity<ECS::LightTag>(name);
    m_ecsManager.AddComponentToEntity<ECS::LightComponent>(m_id, ECS::LightComponent());
}

void Helpers::ECS_Helper::CreateCameraEntity(const string &name) {
    m_id++;
    m_ecsManager.CreateEntity<ECS::CameraTag>(name);
    m_ecsManager.AddComponentToEntity<ECS::CameraComponent>(m_id, ECS::CameraComponent());
}

Galaktic::ECS::Entity& Helpers::ECS_Helper::GetEntityByName(const string &name) const {
    return *m_ecsManager.GetEntityByName(name);
}
