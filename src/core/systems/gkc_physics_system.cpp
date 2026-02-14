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

        rigid_comp.m_force.y += m_gravity * rigid_comp.m_mass;
    }
}

void PhysicsSystem::IntegrateMotion(const ECS::Entity_List& list, float dt) {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>() || !entity.second.Has<ECS::TransformComponent>() )
            continue;

        auto& transform_comp = entity.second.Get<ECS::TransformComponent>();
        auto& rigid_comp = entity.second.Get<ECS::RigidBody>();

        Render::Vec2 acceleration = rigid_comp.m_force / rigid_comp.m_mass;
        rigid_comp.m_velocity += acceleration * dt;
        transform_comp.m_location += rigid_comp.m_velocity * dt;
    }
}

void PhysicsSystem::ResolveGroundCollision(const ECS::Entity_List &list) {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>() || !entity.second.Has<ECS::TransformComponent>())
            continue;

        auto& transform_comp = entity.second.Get<ECS::TransformComponent>();
        auto& rigid_comp = entity.second.Get<ECS::RigidBody>();

        if (transform_comp.m_location.y <= m_floorHeight && m_useFloor) {
            transform_comp.m_location.y = m_floorHeight;
            rigid_comp.m_velocity.y = 0.f;
        }
    }
}

void PhysicsSystem::CleanForces(const ECS::Entity_List& list) {
    for (auto entity : list) {
        if (!entity.second.Has<ECS::RigidBody>() || !entity.second.Has<ECS::TransformComponent>())
            continue;

        entity.second.Get<ECS::RigidBody>().m_force = {0.f, 0.f};
    }
}



