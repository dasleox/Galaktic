#include "core/helpers/gkc_animation_helper.h"
#include <core/managers/gkc_animation_man.h>
#include <core/managers/gkc_ecs_man.h>
#include <render/gkc_animation.h>
#include <ecs/gkc_components.h>

using namespace Galaktic::Core::Helpers;
AnimationHelper::AnimationHelper(Managers::ECS_Manager &ecsManager) 
    : m_ecsManager(ecsManager) {}

void AnimationHelper::SetAnimationToEntity(EntityID id, const string& animationName) {
    auto entity = m_ecsManager.GetEntityByID(id);
    if(entity == nullptr) {
        GKC_ENGINE_ERROR("Entity to apply animation doesn't exist!");
        return;
    }

    auto animation = Managers::AnimationManager::GetAnimationInfo(animationName);
    if(animation == nullptr) {
        GKC_ENGINE_ERROR("Animation '{}' doesn't exist!", animationName);
        return;
    }

    AnimationID animationID = animation->id_;
    
    if(entity->Has<ECS::AnimationComponent>()) {
        auto& animationComp = entity->Get<ECS::AnimationComponent>();
        animationComp.m_id = animationID;
    } else {
        m_ecsManager.AddComponentToEntity<ECS::AnimationComponent>(id, animationID);
    }   
}