#include <core/systems/gkc_movement_system.h>
#include "core/systems/gkc_key.h"
#include "ecs/gkc_entity.h"
#include "ecs/gkc_components.h"
#include <core/gkc_scene.h>
#include <config/gkc_config.h>

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
        
        if(IsActionDown(Config::ActionType::MoveForward, Key::W))
            transform.location.y -= player.maxSpeed * dt;
        if(IsActionDown(Config::ActionType::MoveBackward, Key::S))
            transform.location.y += player.maxSpeed * dt;
        if(IsActionDown(Config::ActionType::MoveLeft, Key::A))
            transform.location.x -= player.maxSpeed * dt;
        if(IsActionDown(Config::ActionType::MoveRight, Key::D))
            transform.location.x += player.maxSpeed * dt;
        if(IsActionDown(Config::ActionType::Jump, Key::Space))
            ApplyJump(entity.second);
    }
}

void MovementSystem::ApplyJump(ECS::Entity &entity) {
    if (entity.Has<ECS::PlayerTag>() && entity.Has<ECS::JumpComponent>() && entity.Has<ECS::RigidBody>()) {
        auto& jump_comp = entity.Get<ECS::JumpComponent>();
        entity.Get<ECS::RigidBody>().force.y += jump_comp.jumpHeight;
    }
}

bool MovementSystem::IsActionDown(Config::ActionType action, Key fallbackKey) const {
    Config::BindType bind = Config::BindConfigurator::GetBindByAction(action);
    if (bind.keyBind != Key::Unknown) {
        return m_keySystem.IsKeyDown(bind.keyBind);
    }

    return m_keySystem.IsKeyDown(fallbackKey);
}