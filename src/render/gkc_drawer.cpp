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

void Drawer::CalculateVertices(SDL_FRect rect, float rotation, SDL_Color color, SDL_Vertex outVerts[4]) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h / 2.0f;
    float rad = rotation * GKC_PI / 180.0f;
    float c = cosf(rad), s = sinf(rad);

    SDL_FPoint corners[4] = {
        { rect.x,          rect.y },
        { rect.x + rect.w, rect.y },
        { rect.x + rect.w, rect.y + rect.h },
        { rect.x,          rect.y + rect.h }
    };

    for (int i = 0; i < 4; i++) {
        float dx = corners[i].x - cx;
        float dy = corners[i].y - cy;

        outVerts[i].position.x = cx + dx * c - dy * s;
        outVerts[i].position.y = cy + dx * s + dy * c;
        outVerts[i].color.r = color.r / 255.0f;
        outVerts[i].color.g = color.g / 255.0f;
        outVerts[i].color.b = color.b / 255.0f;
        outVerts[i].color.a = color.a / 255.0f;
        outVerts[i].tex_coord.x = 0;
        outVerts[i].tex_coord.y = 0;
    }
}

void Drawer::RenderColor(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer) {
    static const int indices[6] = { 0, 1, 2, 0, 2, 3 };
    SDL_Vertex verts[4];
    CalculateVertices(rect, entity.Get<ECS::TransformComponent>().rotation,
        entity.Get<ECS::ColorComponent>().color, verts);
    SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);
}

void Drawer::RenderTexture(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer) {
    using namespace Core::Managers;
    auto& textureComp = entity.Get<ECS::TextureComponent>();
    auto& transformComp = entity.Get<ECS::TransformComponent>();
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
    auto it = TextureManager::GetIDToNameList().find(textureComp.id);
    if (it == TextureManager::GetIDToNameList().end()) {
        sdlTexture = TextureManager::GetMissingTexture();
    }
    
    SDL_RenderTextureRotated(renderer, sdlTexture, NULL ,&rect, transformComp.rotation, NULL, SDL_FLIP_NONE);
}

void Drawer::RenderAnimation(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer) {
    auto& transformComp = entity.Get<ECS::TransformComponent>();
    auto& animationComp = entity.Get<ECS::AnimationComponent>();
    auto animation = Core::Managers::AnimationManager::GetAssetByID(animationComp.id);
    if(animation == nullptr) {
        RenderColor(rect, entity, renderer);
        return;
    }
            
    animation->Render(renderer, rect, transformComp.rotation);
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

        SDL_Vertex verts[4];
        CalculateVertices(rect, transform.rotation, {255, 255, 0, 255}, verts);

        SDL_FPoint outline[5] = {
            verts[0].position, verts[1].position, verts[2].position, verts[3].position, verts[0].position
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow, fully opaque
        SDL_RenderLines(renderer, outline, 5);
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

        SDL_Vertex verts[4];
        CalculateVertices(rect, transform.rotation, {0, 255, 0, 191}, verts);

        SDL_FPoint outline[5] = {
            verts[0].position, verts[1].position, verts[2].position, verts[3].position, verts[0].position
        };

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 191); // Green, 75% alpha
        SDL_RenderLines(renderer, outline, 5);
    }
}