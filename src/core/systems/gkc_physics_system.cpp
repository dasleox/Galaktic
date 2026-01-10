#include <core/systems/gkc_physics_system.h>
#include "ecs/gkc_components.h"
#include "ecs/gkc_entity.h"

using namespace Galaktic::Core::Systems;

void PhysicsSystem::Update(const ECS::Entity_List& list, float dt) {
    ApplyForces(list);
    IntegrateMotion(list, dt);
    ResolveGroundCollision(list);
    CleanForces(list);
}

void PhysicsSystem::ApplyForces(const ECS::Entity_List &list) const {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>())
            continue;

        auto& rigid_comp = entity.second.Get<ECS::RigidBody>();

        if (rigid_comp.use_gravity_) {
            rigid_comp.force_.y += m_gravity * rigid_comp.mass_;
        }
    }
}

void PhysicsSystem::IntegrateMotion(const ECS::Entity_List& list, float dt) {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>() || !entity.second.Has<ECS::TransformComponent>() )
            continue;

        auto& transform_comp = entity.second.Get<ECS::TransformComponent>();
        auto& rigid_comp = entity.second.Get<ECS::RigidBody>();

        Render::Vec2 acceleration = rigid_comp.force_ / rigid_comp.mass_;
        rigid_comp.velocity_ += acceleration * dt;
        transform_comp.location_ += rigid_comp.velocity_ * dt;
    }
}

void PhysicsSystem::ResolveGroundCollision(const ECS::Entity_List &list) {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>() || !entity.second.Has<ECS::TransformComponent>())
            continue;

        auto& transform_comp = entity.second.Get<ECS::TransformComponent>();
        auto& rigid_comp = entity.second.Get<ECS::RigidBody>();

        if (transform_comp.location_.y <= 0.f) {
            transform_comp.location_.y = 0.f;
            rigid_comp.velocity_.y = 0.f;
        }
    }
}

void PhysicsSystem::CleanForces(const ECS::Entity_List& list) {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>() || !entity.second.Has<ECS::TransformComponent>())
            continue;

        entity.second.Get<ECS::RigidBody>().velocity_ = {0.f, 0.f};
    }
}

void PhysicsSystem::ApplyJump(ECS::Entity &entity) {
    if (entity.Has<ECS::PlayerTag>() && entity.Has<ECS::JumpComponent>()) {
        auto& jump_comp = entity.Get<ECS::JumpComponent>();
        entity.Get<ECS::RigidBody>().force_.y += jump_comp.jump_height_;
    }
}



