#include <core/systems/gkc_movement_system.h>
#include "core/systems/gkc_key.h"
#include "ecs/gkc_entity.h"
#include "ecs/gkc_components.h"
#include <core/gkc_scene.h>

#include "core/gkc_logger.h"

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;
using namespace Galaktic::ECS;

void MovementSystem::Update(const Entity_List& list, float dt) {
    for (auto entity : list) {
        if (!entity.second.Has<TransformComponent>() || !entity.second.Has<SpeedComponent>()
            || !entity.second.Has<PlayerTag>())
            continue;

        auto& transform = entity.second.Get<TransformComponent>();
        auto& player = entity.second.Get<SpeedComponent>();
        if (player.max_speed_ * dt == player.max_speed_) {
            continue;
        }
        // Movement Updates

        if (m_keySystem.IsKeyDown(Key::W))
            transform.location_.y -= player.max_speed_ * dt;
        if (m_keySystem.IsKeyDown(Key::S))
            transform.location_.y += player.max_speed_ * dt;
        if (m_keySystem.IsKeyDown(Key::A))
            transform.location_.x -= player.max_speed_ * dt;
        if (m_keySystem.IsKeyDown(Key::D))
            transform.location_.x += player.max_speed_   * dt;
    }
}