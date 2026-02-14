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
    void ClearCheckedEntities() {
        checkedEntities.clear();
    }
}

void Drawer::DrawEntities(const ECS::Entity_List& list, SDL_Renderer *renderer,
    Core::Systems::CameraSystem& cameraSystem)
{
    using namespace Core::Managers;
    auto& camera = cameraSystem.GetActiveCamera().Get<ECS::CameraComponent>();

    ClearCheckedEntities();

    for (auto entity : list) {
        auto& name = entity.second.Get<ECS::NameComponent>().m_name;

        if (!checkedEntities.contains(entity.first)) {
            if (!entity.second.IsValid()) {
                checkedEntities[entity.first] = true;
                GKC_ENGINE_WARNING("{0} is not a valid entity", name);
                continue;
            }
            checkedEntities[entity.first] = true;
        }
        if (entity.second.Has<ECS::LightTag>() || entity.second.Has<ECS::CameraComponent>()) continue;

        auto& transform = entity.second.Get<ECS::TransformComponent>();
        SDL_FRect rect;
        rect.w = transform.m_size.x;
        rect.h = transform.m_size.y;

        rect.x = transform.m_location.x - camera.m_location.x;
        rect.y = transform.m_location.y - camera.m_location.y;

        // Render texture if it has texture
        if (entity.second.Has<ECS::TextureComponent>()) {
            auto& textureComp = entity.second.Get<ECS::TextureComponent>();
            auto texture = TextureManager::GetTextureByID(textureComp.m_id);
            SDL_Texture* sdlTexture = nullptr;
            
            if(texture == nullptr) {
                sdlTexture = TextureManager::GetMissingTexture();
            } else {
                sdlTexture = texture->GetSDLTexture();
                if(sdlTexture == nullptr) {
                    sdlTexture = TextureManager::GetMissingTexture();
                }
            }

            auto& textureName = TextureManager::GetIDTextureList().find(textureComp.m_id)->second;
            SDL_RenderTexture(renderer, sdlTexture, NULL ,&rect);
        } 
        
        else if (entity.second.Has<ECS::AnimationComponent>()) {
            auto& animationComp = entity.second.Get<ECS::AnimationComponent>();
            auto animation = AnimationManager::GetAnimation(animationComp.m_id);
            if(animation == nullptr) {
                // Programming Warcrime
                goto color_rendering;
            }
            
            animation->Render(renderer, rect);
        }

        // Color Rendering
        else {
            color_rendering:
            auto& color = entity.second.Get<ECS::ColorComponent>().m_color;
            SDL_SetRenderDrawColor(renderer, GKC_SET_COLOR(color));
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}