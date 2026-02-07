#include <core/managers/gkc_texture_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"
#include "render/gkc_texture.h"
#include "ecs/gkc_entity.h"
#include "core/managers/gkc_texture_man.h"

using namespace Galaktic::Core;

Galaktic::Render::Texture_List Managers::TextureManager::m_textureList;
Galaktic::Render::TextureID_List Managers::TextureManager::m_IDToNameList;
vector<path> Managers::TextureManager::m_texturePathList;
SDL_Texture* Managers::TextureManager::m_missingTexture = nullptr;

Managers::TextureManager::TextureManager(const path &path) {
    auto files = Filesystem::GetFilenamesInFolder(path);
    for (auto& file : files) {
        if (Render::CheckTextureExtension(file))
            AddTexturePath(file);
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
    m_texturePathList.emplace_back(path);

    GKC_ENGINE_INFO("'{}' texture added and loaded sucessfully!", path.filename().string());
}

void Managers::TextureManager::AddTexturePath(const path& path) {
    TextureID id = m_textureList.size() + 1;
    auto info = make_unique<Render::TextureInfo>(id, nullptr);
    if(info == nullptr || info->id_ == 0) {
        GKC_ENGINE_ERROR("Failed to add texture at path: {0}", path.string());
        return;
    }

    auto [it, inserted] = m_textureList.emplace(Filesystem::GetFilename(path), std::move(info));
    m_IDToNameList.emplace(id, Filesystem::GetFilename(path));
    m_texturePathList.emplace_back(path);

    GKC_ENGINE_INFO("'{}' texture added sucessfully!", path.filename().string());
    GKC_ENGINE_INFO("REMINDER: '{}' has to be loaded before use", path.filename().string());
}

void Managers::TextureManager::LoadTexture(const path& path, SDL_Renderer* renderer) {
    string textureName = Filesystem::GetFilename(path);
    auto it = m_textureList.find(textureName);
    if (it != m_textureList.end()) {
        TextureID id = it->second->id_;
        auto texture = make_unique<Render::Texture>(path, renderer);
        if(texture == nullptr) {
            GKC_ENGINE_ERROR("Failed to load texture at path: {0}", path.string());
            return;
        }

        it->second.get()->texture_ = std::move(texture);
        GKC_ENGINE_INFO("'{}' texture loaded successfully", textureName);
    } else {
        AddTexture(path, renderer);
    }
}

void Managers::TextureManager::LoadAllTextures(SDL_Renderer* renderer) {
    for(auto& path : m_texturePathList) {
        LoadTexture(path, renderer);
    }
    PrintList();
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
        if(texture->second != nullptr) {
            return texture->second->texture_.get();
        }
        GKC_ENGINE_WARNING("Texture key exist but hasn't been loaded yet!");
        return nullptr;
    }
    GKC_ENGINE_WARNING("Texture doesn't exist!");
    return nullptr;
}

Galaktic::Render::Texture * Managers::TextureManager::GetTexture(TextureID id) {
    auto nameToFind = m_IDToNameList.find(id);
    if (nameToFind != m_IDToNameList.end()) {
        return GetTexture(nameToFind->second);;
    }
    GKC_ENGINE_WARNING("Texture doesn't exist");
    return nullptr;
}

Galaktic::Render::TextureInfo *Galaktic::Core::Managers::TextureManager::GetTextureInfo(const string &textureName) {
    auto it = m_textureList.find(textureName);
    if(it != m_textureList.end()) {
        if(it->second != nullptr) {
            return it->second.get();
        }
        GKC_ENGINE_WARNING("Texture Information is NULL!");
        return nullptr;
    }
    GKC_ENGINE_WARNING("Texture Information doesn't exist");
    return nullptr;
}

void Managers::TextureManager::PrintList() {
    /// So many nullptrs that can cause trouble dude
    for (auto& pair : m_textureList) {
        const void* textureAddress = TakeAdressOfTexture(pair.second.get());

        if(textureAddress == nullptr) {
            GKC_ENGINE_INFO("Key: {0} | {1} Address: {2}", pair.first, pair.second->id_, "nullptr (needs to be loaded)");
        } else {
            GKC_ENGINE_INFO("Key: {0} | {1} Address: {2}", pair.first, pair.second->id_, textureAddress);
        }
    }
}

const void* Managers::TextureManager::TakeAdressOfTexture(const Render::TextureInfo* textureInfo) {
    if(textureInfo != nullptr) {
        auto& texture = textureInfo->texture_;
        if(texture != nullptr) {
            return CastToVoidPtr(texture);
        }
        return nullptr;
    }
    return nullptr;
}

SDL_Texture* Managers::TextureManager::GetMissingTexture() {
    return m_missingTexture;
}

void Galaktic::Core::Managers::TextureManager::CreateMissingTexture(SDL_Renderer* renderer){
    const int size = 16;
    const int checkerSize = 4;
    vector<Uint32> pixels(size*size);
    for(int y = 0; y < size; y++) {
        for(int x = 0; x < size; x++) {
            bool isChecker = ((x / checkerSize) + (y / checkerSize)) % 2 == 0;
            pixels[y * size + x] = isChecker ? 0xFFFF00FF : 0xFF000000;
        }
    }

    SDL_Surface* surface = SDL_CreateSurfaceFrom(size, size, SDL_PIXELFORMAT_RGBA8888, pixels.data(), size * 4);
    m_missingTexture = SDL_CreateTextureFromSurface(renderer, surface);
    static std::vector<Uint32> persistentPixels = pixels;
    SDL_DestroySurface(surface);

    GKC_RELEASE_ASSERT(m_missingTexture != nullptr, "Even the missing texture failed to be created, you are fucked :)");
}