#include <core/managers/gkc_texture_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"
#include "render/gkc_texture.h"

using namespace Galaktic::Core;

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
    auto [it, inserted] = m_textureList.emplace(Filesystem::GetFilename(path), std::move(info));

    #ifdef GKC_PRINT_TEXTURE_ADDED
        GKC_ENGINE_INFO("[{0} | ID: {1}] at address {2}", path.string(), id,
            CastToVoidPtr(it->second->texture_.operator*()));
    #endif
}

void Managers::TextureManager::DeleteTexture(const string& name) {
    auto texture = m_textureList.find(name);
    if (texture != m_textureList.end()) {
        m_textureList.erase(texture);
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

void Managers::TextureManager::PrintList() const {
    for (auto& pair : m_textureList) {
        GKC_ENGINE_INFO("Key: {0} | {1} &{2}", pair.first, pair.second->id_,
            CastToVoidPtr(pair.second->texture_.operator*()));
    }
}
