#include <render/gkc_drawer.h>
#include "core/gkc_logger.h"
#include "core/managers/gkc_texture_man.h"
#include "core/systems/gkc_camera_system.h"
#include "ecs/gkc_components.h"
#include "ecs/gkc_entity.h"
#include "render/gkc_texture.h"

using namespace Galaktic::Render;

namespace {
    unordered_map<EntityID, bool> checkedEntities;
}

void Drawer::DrawEntities(const ECS::Entity_List& list, SDL_Renderer *renderer,
    Core::Systems::CameraSystem& cameraSystem)
{
    using namespace Core::Managers;
    auto& camera = cameraSystem.GetActiveCamera().Get<ECS::CameraComponent>();

    for (auto entity : list) {
        auto& name = entity.second.Get<ECS::NameComponent>().name_;
        checkedEntities[entity.first] = true;

        // Avoids re-logging of the same invalid/not-renderable entity
        if (!checkedEntities.contains(entity.first) ) {
            if (!entity.second.IsValid()) {
                GKC_ENGINE_WARNING("{0} is not a valid entity", name);
                continue;
            }
        }
        if (entity.second.Has<ECS::LightTag>() || entity.second.Has<ECS::CameraComponent>()) continue;

        auto& transform = entity.second.Get<ECS::TransformComponent>();
        SDL_FRect rect;
        rect.w = transform.size_.x;
        rect.h = transform.size_.y;

        rect.x = transform.location_.x - camera.location_.x;
        rect.y = transform.location_.y - camera.location_.y;

        // Render texture if it has texture
        // @TODO make a class to automatically assign the entities a texture by ID
        if (entity.second.Has<ECS::TextureComponent>()) {
            auto& texture = entity.second.Get<ECS::TextureComponent>();
            auto sdlTexture = TextureManager::GetTexture(texture.id_)->GetSDLTexture();
            auto& textureName = TextureManager::GetIDTextureList().find(texture.id_)->second;

            // If the texture doesn't exist, log an error and display color instead
            if (sdlTexture == nullptr) {
                if (!checkedEntities.contains(entity.first) ) {
                    GKC_ENGINE_ERROR("('{0}' ID: {1}) has an invalid texture: '{2}'",
                    name, entity.first, textureName);
                    SDL_RenderTexture(renderer, NULL, NULL ,&rect);
                    continue;
                }
            }

            SDL_RenderTexture(renderer, sdlTexture, NULL ,&rect);
        }

        // Color Rendering
        else {
            auto& color = entity.second.Get<ECS::ColorComponent>().color_;
            SDL_SetRenderDrawColor(renderer, GKC_SET_COLOR(color));
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}