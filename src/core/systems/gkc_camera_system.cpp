#include <core/systems/gkc_camera_system.h>
#include "ecs/gkc_components.h"
#include "core/gkc_logger.h"
#include "ecs/gkc_entity.h"

using namespace Galaktic::Core;

Systems::CameraSystem::CameraSystem(ECS::Entity& camera) : m_activeCamera(camera) {}

void Systems::CameraSystem::Update(const ECS::Entity_List& list, float dt,
    Uint32 width, Uint32 height) {
    FindPrimaryCamera(list);

    GKC_ASSERT(m_activeCamera.IsValid(), "No cameras exist!");

    for (auto pair : list) {
        auto& entity = pair.second;
        auto& id = pair.first;
        if (!m_activeCamera.Has<ECS::CameraComponent>()) {
            GKC_ENGINE_WARNING("Why the fuck a camera is not a camera?");
            continue;
        }

        auto& cameraComp = m_activeCamera.Get<ECS::CameraComponent>();

        if (entity.Has<ECS::TransformComponent>() && cameraComp.m_entityToFollowID == id
            && cameraComp.m_isActive) {
            auto& transform = entity.Get<ECS::TransformComponent>();
            Render::Vec2 desiredLocation;
            desiredLocation.x = transform.m_location.x - static_cast<float>(width)  * 0.5f;
            desiredLocation.y = transform.m_location.y - static_cast<float>(height) * 0.5f;

            cameraComp.m_location.x = std::lerp(cameraComp.m_location.x, desiredLocation.x,
                cameraComp.m_smoothing * dt);
            cameraComp.m_location.y = std::lerp(cameraComp.m_location.y, desiredLocation.y,
                cameraComp.m_smoothing * dt);
        }
    }
}

void Systems::CameraSystem::SetFollowEntity(EntityID id) {
    m_activeCamera.Get<ECS::CameraComponent>().m_entityToFollowID = id;
}

void Systems::CameraSystem::FindPrimaryCamera(const ECS::Entity_List& list) {
    for (auto pair : list) {
        auto& entity = pair.second;

        if (entity.Has<ECS::CameraComponent>()) {
            auto& cameraComp = entity.Get<ECS::CameraComponent>();
            if (cameraComp.m_isActive) {
                m_activeCamera = entity;
                break;
            }
        }
    }
}
