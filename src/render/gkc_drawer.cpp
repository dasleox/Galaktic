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
using namespace Galaktic;

void Drawer::DrawEntities(const ECS::Entity_List& list, SDL_Renderer *renderer,
    Core::Systems::CameraSystem& cameraSystem)
{
    using namespace Core::Managers;
    auto& camera = cameraSystem.GetActiveCamera().Get<ECS::CameraComponent>();

    for (auto [id, entity] : list) {
        auto& name = entity.Get<ECS::NameComponent>().name;

        if (entity.Has<ECS::LightTag>() || entity.Has<ECS::CameraComponent>()
            || id == InvalidEntity) continue;

        auto& transform = entity.Get<ECS::TransformComponent>();
        SDL_FRect rect;
        rect.w = transform.size.x;
        rect.h = transform.size.y;

        rect.x = transform.location.x - camera.location.x;
        rect.y = transform.location.y - camera.location.y;

        if (entity.Has<ECS::AnimationComponent>()) {
            RenderAnimation(rect, entity, renderer);
        } 
        else if (entity.Has<ECS::TextureComponent>()) {
            RenderTexture(rect, entity, renderer);
        } else {
            RenderColor(rect, entity, renderer);
        }
    }
}

void Drawer::RenderColor(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer) {
    auto& color = entity.Get<ECS::ColorComponent>().color;
    SDL_SetRenderDrawColor(renderer, GKC_SET_COLOR(color));
    SDL_RenderFillRect(renderer, &rect);
}

void Drawer::RenderTexture(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer) {
    using namespace Core::Managers;
    auto& textureComp = entity.Get<ECS::TextureComponent>();
    auto texture = TextureManager::GetAssetByID(textureComp.id);
    SDL_Texture* sdlTexture = nullptr;
            
    if(texture == nullptr) {
        sdlTexture = TextureManager::GetMissingTexture();
    } else {
        sdlTexture = texture->GetSDLTexture();

        if(sdlTexture == nullptr) {
            sdlTexture = TextureManager::GetMissingTexture();
        }
    }

    // FIXED: Add NULL check before dereferencing iterator
    auto it = TextureManager::GetIDToNameList().find(textureComp.id);
    if (it == TextureManager::GetIDToNameList().end()) {
        sdlTexture = TextureManager::GetMissingTexture();
    }
    
    SDL_RenderTexture(renderer, sdlTexture, NULL ,&rect);
}

void Drawer::RenderAnimation(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer) {
    auto& animationComp = entity.Get<ECS::AnimationComponent>();
    auto animation = Core::Managers::AnimationManager::GetAssetByID(animationComp.id);
    if(animation == nullptr) {
        RenderColor(rect, entity, renderer);
    }
            
    animation->Render(renderer, rect);
}

void Drawer::DrawWireframes(const ECS::Entity_List& list, SDL_Renderer* renderer,
    Core::Systems::CameraSystem& cameraSystem)
{
    auto& camera = cameraSystem.GetActiveCamera().Get<ECS::CameraComponent>();

    for (auto [id, entity] : list) {
        if (entity.Has<ECS::LightTag>() || entity.Has<ECS::CameraComponent>()
            || id == InvalidEntity) continue;

        auto& transform = entity.Get<ECS::TransformComponent>();
        SDL_FRect rect;
        rect.w = transform.size.x;
        rect.h = transform.size.y;
        rect.x = transform.location.x - camera.location.x;
        rect.y = transform.location.y - camera.location.y;

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow, fully opaque
        SDL_RenderRect(renderer, &rect);
    }
}

void Drawer::DrawColliders(const ECS::Entity_List& list, SDL_Renderer* renderer,
    Core::Systems::CameraSystem& cameraSystem)
{
    auto& camera = cameraSystem.GetActiveCamera().Get<ECS::CameraComponent>();

    for (auto [id, entity] : list) {
        if (entity.Has<ECS::LightTag>() || entity.Has<ECS::CameraComponent>()
            || id == InvalidEntity) continue;

        if (!entity.Has<ECS::CollisionComponent>()) continue;

        auto& transform    = entity.Get<ECS::TransformComponent>();
        auto& collisionBox = entity.Get<ECS::CollisionComponent>().collisionBox;

        SDL_FRect rect;
        rect.w = collisionBox.x;
        rect.h = collisionBox.y;
        rect.x = (transform.location.x + collisionBox.x) - camera.location.x;
        rect.y = (transform.location.y + collisionBox.y) - camera.location.y;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 191); // Green, 75% alpha (255 * 0.75 = 191)
        SDL_RenderRect(renderer, &rect);
    }
}