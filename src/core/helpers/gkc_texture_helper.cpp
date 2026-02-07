#include <core/helpers/gkc_texture_helper.h>
#include <core/managers/gkc_ecs_man.h>
#include <ecs/gkc_components.h>
#include <core/managers/gkc_texture_man.h>
#include <render/gkc_texture.h>

using namespace Galaktic::Core;

Helpers::TextureHelper::TextureHelper(Managers::ECS_Manager& ecsManager) 
    :  m_ecsManager(ecsManager) {}

void Helpers::TextureHelper::SetTextureToEntity(EntityID id, const string& textureName) {
    auto entity = m_ecsManager.GetEntityByID(id);
    if(entity == nullptr) {
        GKC_ENGINE_ERROR("Entity to apply texture doesn't exist!");
        return;
    }

    auto texture = Managers::TextureManager::GetTextureInfo(textureName);
    if(texture == nullptr) {
        GKC_ENGINE_ERROR("Texture '{}' doesn't exist!", textureName);
        return;
    }

    TextureID textureID = texture->id_;
    
    if(entity->Has<ECS::TextureComponent>()) {
        auto& textureComp = entity->Get<ECS::TextureComponent>();
        textureComp.id_ = textureID;
    } else {
        m_ecsManager.AddComponentToEntity<ECS::TextureComponent>(id, textureID);
    }   

}

