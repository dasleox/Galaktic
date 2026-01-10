#include <render/gkc_drawer.h>
#include "core/gkc_logger.h"
#include "ecs/gkc_components.h"
#include "ecs/gkc_entity.h"

using namespace Galaktic::Render;

namespace {
    unordered_map<EntityID, bool> checkedEntities;
}

void Drawer::DrawEntities(const ECS::Entity_List& list, SDL_Renderer *renderer) {
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
        if (entity.second.Has<ECS::LightTag>()) continue;
        auto& transform = entity.second.Get<ECS::TransformComponent>();
        auto& color = entity.second.Get<ECS::ColorComponent>().color_;

        SDL_FRect objectRect = {GKC_VEC2_DECOUPLE(transform.location_), GKC_VEC2_DECOUPLE(transform.size_)};
        SDL_SetRenderDrawColor(renderer, GKC_SET_COLOR(color));
        SDL_RenderFillRect(renderer, &objectRect);
    }
}