#include "core/helpers/gkc_animation_helper.h"
#include <core/managers/gkc_animation_man.h>
#include <core/managers/gkc_ecs_man.h>
#include <render/gkc_animation.h>
#include <core/gkc_error.h>
#include <ecs/gkc_components.h>

using namespace Galaktic::Core::Helpers;
using namespace Galaktic::Core::Managers;
AnimationHelper::AnimationHelper(Managers::ECS_Manager &ecsManager) 
    : m_ecsManager(ecsManager) {}

void AnimationHelper::SetAnimationToEntity(EntityID id, const string& animationName) {
    auto entity = m_ecsManager.GetEntityByID(id);
    if(entity == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::EntityNotFound, "Entity (ID: {}) to apply animation doesn't exist!", id);
        return;
    }

    auto animation = Managers::AnimationManager::GetAssetInfo(animationName);
    if(animation == nullptr)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound, "Animation '{}' not found!", animationName);
        return;
    }

    auto animationID = AnimationManager::GetAssetIDByName(animationName);

    if(animationID == 0)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid animation ID!: {}", animationName);
        return;
    }
    
    if(entity->Has<ECS::AnimationComponent>()) {
        auto& animationComp = entity->Get<ECS::AnimationComponent>();
        animationComp.id = animationID;
    } else {
        m_ecsManager.AddComponentToEntity<ECS::AnimationComponent>(id, animationID);
    }   
}