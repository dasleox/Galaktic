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
        // Movement Updates
        if (player.m_maxSpeed * dt == player.m_maxSpeed) {
            continue;
        }

        // Modify this to use the Player.lua file
        if (m_keySystem.IsKeyDown(Key::W))
            transform.m_location.y -= player.m_maxSpeed * dt;
        if (m_keySystem.IsKeyDown(Key::S))
            transform.m_location.y += player.m_maxSpeed * dt;
        if (m_keySystem.IsKeyDown(Key::A))
            transform.m_location.x -= player.m_maxSpeed * dt;
        if (m_keySystem.IsKeyDown(Key::D))
            transform.m_location.x += player.m_maxSpeed   * dt;
        if(m_keySystem.IsKeyDown(Key::Space))
            ApplyJump(entity.second);
    }
}

void MovementSystem::ApplyJump(ECS::Entity &entity) {
    if (entity.Has<ECS::PlayerTag>() && entity.Has<ECS::JumpComponent>() && entity.Has<ECS::RigidBody>()) {
        auto& jump_comp = entity.Get<ECS::JumpComponent>();
        entity.Get<ECS::RigidBody>().m_force.y += jump_comp.m_jumpHeight;
    }
}