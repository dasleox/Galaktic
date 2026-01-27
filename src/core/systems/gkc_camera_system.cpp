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

        if (entity.Has<ECS::TransformComponent>() && cameraComp.entityToFollowID_ == id
            && cameraComp.active_) {
            auto& transform = entity.Get<ECS::TransformComponent>();
            Render::Vec2 desiredLocation;
            desiredLocation.x = transform.location_.x - static_cast<float>(width)  * 0.5f;
            desiredLocation.y = transform.location_.y - static_cast<float>(height) * 0.5f;

            cameraComp.location_.x = std::lerp(cameraComp.location_.x, desiredLocation.x,
                cameraComp.smoothing_ * dt);
            cameraComp.location_.y = std::lerp(cameraComp.location_.y, desiredLocation.y,
                cameraComp.smoothing_ * dt);
        }
    }
}

void Systems::CameraSystem::SetFollowEntity(EntityID id) {
    m_activeCamera.Get<ECS::CameraComponent>().entityToFollowID_ = id;
}

void Systems::CameraSystem::FindPrimaryCamera(const ECS::Entity_List& list) {
    for (auto pair : list) {
        auto& entity = pair.second;

        if (entity.Has<ECS::CameraComponent>()) {
            auto& cameraComp = entity.Get<ECS::CameraComponent>();
            if (cameraComp.active_) {
                m_activeCamera = entity;
                break;
            }
        }
    }
}
