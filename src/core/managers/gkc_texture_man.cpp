#include <core/managers/gkc_texture_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"
#include "render/gkc_texture.h"
#include "ecs/gkc_entity.h"

using namespace Galaktic::Core;

Galaktic::Render::Texture_List Managers::TextureManager::m_textureList;
Galaktic::Render::TextureID_List Managers::TextureManager::m_IDToNameList;

Managers::TextureManager::TextureManager(const path &path, SDL_Renderer *renderer) {
    auto files = Filesystem::GetFilenamesInFolder(path);
    for (auto& file : files) {
        if (Render::CheckTextureExtension(file))
            AddTexture(file, renderer);
    }
}

void Managers::TextureManager::AddTexture(const path& path, SDL_Renderer* renderer) {
    TextureID id = m_textureList.size() + 1;
    auto info = make_unique<Render::TextureInfo>(id, make_unique<Render::Texture>(path, renderer));
    if(!info->texture_->IsValid() || !info->id_ == 0) {
        GKC_ENGINE_ERROR("Failed to load texture at path: {0}", path.string());
        return;
    }
    
    auto [it, inserted] = m_textureList.emplace(Filesystem::GetFilename(path), std::move(info));
    m_IDToNameList.emplace(id, Filesystem::GetFilename(path));

    #ifdef GKC_PRINT_TEXTURE_ADDED
        GKC_ENGINE_INFO("[{0} | ID: {1}] at address {2}", path.string(), id,
            CastToVoidPtr(it->second->texture_.operator*()));
    #endif
}

void Managers::TextureManager::DeleteTexture(const string& name) {
    auto texture = m_textureList.find(name);
    if (texture != m_textureList.end()) {
        m_textureList.erase(texture);
        m_IDToNameList.erase(texture->second->id_);
        GKC_ENGINE_INFO("Erased {0}", name);
    }
}

Galaktic::Render::Texture* Managers::TextureManager::GetTexture(const string &name) {
    auto texture = m_textureList.find(name);
    if (texture != m_textureList.end()) {
        return texture->second->texture_.get();
    }
    GKC_ENGINE_WARNING("Texture doesn't exist");
    return nullptr;
}

Galaktic::Render::Texture * Managers::TextureManager::GetTexture(TextureID id) {
    auto nameToFind = m_IDToNameList.find(id);
    if (nameToFind != m_IDToNameList.end()) {
        auto texture = m_textureList.find(nameToFind->second);
        if (texture != m_textureList.end()) {
            return texture->second->texture_.get();
        }
    }
    GKC_ENGINE_WARNING("Texture doesn't exist");
    return nullptr;
}

void Managers::TextureManager::PrintList() {
    for (auto& pair : m_textureList) {
        GKC_ENGINE_INFO("Key: {0} | {1} &{2}", pair.first, pair.second->id_,
            CastToVoidPtr(pair.second->texture_.operator*()));
    }
}
