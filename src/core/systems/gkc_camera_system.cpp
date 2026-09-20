#include <core/systems/gkc_camera_system.h>
#include "ecs/gkc_components.h"
#include "core/gkc_logger.h"
#include "ecs/gkc_entity.h"

using namespace Galaktic::Core;

Systems::CameraSystem::CameraSystem(ECS::Entity& camera) : m_activeCamera(camera) {}

void Systems::CameraSystem::Update(const ECS::Entity_List& list, float dt,
    Uint32 width, Uint32 height) {
    FindPrimaryCamera(list);

    for (auto pair : list) {
        auto& entity = pair.second;
        auto& id = pair.first;
        if (!m_activeCamera.Has<ECS::CameraComponent>()) {
            GKC_ENGINE_WARNING("Why the fuck a camera is not a camera?");
            continue;
        }

        auto& cameraComp = m_activeCamera.Get<ECS::CameraComponent>();

        if (entity.Has<ECS::TransformComponent>() && cameraComp.entityToFollowID == id
            && cameraComp.isActive) {
            auto& transform = entity.Get<ECS::TransformComponent>();
            Render::Vec2 desiredLocation;
            desiredLocation.x = transform.location.x - static_cast<float>(width)  * 0.5f;
            desiredLocation.y = transform.location.y - static_cast<float>(height) * 0.5f;

            cameraComp.location.x = std::lerp(cameraComp.location.x, desiredLocation.x,
                cameraComp.smoothing * dt);
            cameraComp.location.y = std::lerp(cameraComp.location.y, desiredLocation.y,
                cameraComp.smoothing * dt);
        }
    }
}

void Systems::CameraSystem::SetFollowEntity(EntityID id) {
    m_activeCamera.Get<ECS::CameraComponent>().entityToFollowID = id;
}

void Systems::CameraSystem::FindPrimaryCamera(const ECS::Entity_List& list) {
    for (auto pair : list) {
        auto& entity = pair.second;

        if (entity.Has<ECS::CameraComponent>()) {
            auto& cameraComp = entity.Get<ECS::CameraComponent>();
            if (cameraComp.isActive) {
                m_activeCamera = entity;
                break;
            }
        }
    }
}
