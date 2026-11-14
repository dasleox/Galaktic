#include <core/systems/gkc_movement_system.h>
#include "core/systems/gkc_key.h"
#include "ecs/gkc_entity.h"
#include "ecs/gkc_components.h"
#include <core/gkc_scene.h>
#include <config/gkc_config.h>

#include "core/gkc_logger.h"
#include <cmath>

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;
using namespace Galaktic::ECS;

void MovementSystem::Update(const Entity_List& list, float dt) {
    for (auto [id, entity] : list) {
        if (!entity.Has<TransformComponent>() || !entity.Has<SpeedComponent>() ||
            !entity.Has<PlayerTag>()) continue;
        Render::Vec2 direction;
        if (IsActionDown(Config::ActionType::MoveForward, Key::W)) direction.y -= 1.f;
        if (IsActionDown(Config::ActionType::MoveBackward, Key::S)) direction.y += 1.f;
        if (IsActionDown(Config::ActionType::MoveLeft, Key::A)) direction.x -= 1.f;
        if (IsActionDown(Config::ActionType::MoveRight, Key::D)) direction.x += 1.f;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.f) direction = direction / length;
        auto velocity = direction * entity.Get<SpeedComponent>().maxSpeed;
        if (entity.Has<RigidBody>()) {
            auto& body = entity.Get<RigidBody>();
            if (body.bodyType != BodyType::Static) body.velocity = velocity;
        } else {
            entity.Get<TransformComponent>().location += velocity * dt;
        }
    }
}

bool MovementSystem::IsActionDown(Config::ActionType action, Key fallbackKey) const {
    Config::BindType bind = Config::BindConfigurator::GetBindByAction(action);
    if (bind.keyBind != Key::Unknown) {
        return m_keySystem.IsKeyDown(bind.keyBind);
    }

    return m_keySystem.IsKeyDown(fallbackKey);
}