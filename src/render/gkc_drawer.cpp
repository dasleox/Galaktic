#include <render/gkc_drawer.h>
#include "core/gkc_logger.h"
#include "core/managers/gkc_texture_man.h"
#include "core/managers/gkc_animation_man.h"
#include "core/systems/gkc_camera_system.h"
#include "ecs/gkc_components.h"
#include "ecs/gkc_entity.h"
#include "render/gkc_texture.h"
#include "render/gkc_animation.h"

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
        if (entity.second.Has<ECS::TextureComponent>()) {
            auto& textureComp = entity.second.Get<ECS::TextureComponent>();
            Render::Texture* texture = TextureManager::GetTexture(textureComp.id_);
            SDL_Texture* sdlTexture = nullptr;
            
            if(texture == nullptr) {
                sdlTexture = TextureManager::GetMissingTexture();
            } else {
                sdlTexture = texture->GetSDLTexture();
                if(sdlTexture == nullptr) {
                    sdlTexture = TextureManager::GetMissingTexture();
                }
            }

            auto& textureName = TextureManager::GetIDTextureList().find(textureComp.id_)->second;
            SDL_RenderTexture(renderer, sdlTexture, NULL ,&rect);
        } 
        
        else if (entity.second.Has<ECS::AnimationComponent>()) {
            auto& animationComp = entity.second.Get<ECS::AnimationComponent>();
            Render::Animation* animation = AnimationManager::GetAnimation(animationComp.id_);
            if(animation == nullptr) {
                // Programming Warcrime
                goto color_rendering;
            }
            
            animation->Render(renderer, rect);
        }

        // Color Rendering
        else {
            color_rendering:
            auto& color = entity.second.Get<ECS::ColorComponent>().color_;
            SDL_SetRenderDrawColor(renderer, GKC_SET_COLOR(color));
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}