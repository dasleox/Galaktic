#include <core/managers/gkc_texture_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"
#include "render/gkc_texture.h"
#include "ecs/gkc_entity.h"
#include "core/managers/gkc_texture_man.h"

using namespace Galaktic::Core;
using namespace Galaktic;

Galaktic::Render::Texture_List Managers::TextureManager::m_textureList;
Galaktic::Render::TextureID_List Managers::TextureManager::m_IDToNameList;
vector<path> Managers::TextureManager::m_texturePathList;
SDL_Texture* Managers::TextureManager::m_missingTexture = nullptr;

Managers::TextureManager::TextureManager(const string &path) {
    auto files = Filesystem::GetFilenamesInFolder(path);
    for (auto& file : files) {
        if (Render::CheckTextureExtension(file))
            AddTexturePath(file);
    }
}

void Managers::TextureManager::AddTexture(const string& path, SDL_Renderer* renderer) {
    TextureID id = m_textureList.size() + 1;
    auto info = make_shared<Render::TextureInfo>(id, make_shared<Render::Texture>(path, renderer));
    if(!info->texture_->IsValid() || info->id_ == 0) {
        GKC_ENGINE_ERROR("Failed to load texture at path: {0}", path);
        return;
    }
    
    string textureName = Filesystem::GetFilename(path);
    auto [it, inserted] = m_textureList.emplace(textureName, std::move(info));
    m_IDToNameList.emplace(id, textureName);
    m_texturePathList.emplace_back(path);

    GKC_ENGINE_INFO("'{}' texture added and loaded sucessfully!", path);
}

void Managers::TextureManager::AddTexturePath(const string& path) {
    TextureID id = m_textureList.size() + 1;
    auto info = make_shared<Render::TextureInfo>(id, nullptr);
    if(info == nullptr || info->id_ == 0) {
        GKC_ENGINE_ERROR("Failed to add texture at path: {0}", path);
        return;
    }

    string textureName = Filesystem::GetFilename(path);
    auto [it, inserted] = m_textureList.emplace(textureName, std::move(info));
    m_IDToNameList.emplace(id, textureName);
    m_texturePathList.emplace_back(path);

    GKC_ENGINE_INFO("'{}' texture added sucessfully!", path);
    GKC_ENGINE_INFO("REMINDER: '{}' has to be loaded before use", path);
}

void Managers::TextureManager::LoadTexture(const string& path, SDL_Renderer* renderer) {
    string textureName = Filesystem::GetFilename(path);
    auto it = m_textureList.find(textureName);
    if (it != m_textureList.end()) {
        TextureID id = it->second->id_;
        auto texture = make_shared<Render::Texture>(path, renderer);
        if(texture == nullptr) {
            GKC_ENGINE_ERROR("Failed to load texture at path: {0}", path);
            return;
        }

        it->second->texture_ = std::move(texture);
        GKC_ENGINE_INFO("'{}' texture loaded successfully", textureName);
    } else {
        AddTexture(path, renderer);
    }
}

void Managers::TextureManager::LoadAllTextures(SDL_Renderer* renderer) {
    for(auto& path : m_texturePathList) {
        LoadTexture(path.string(), renderer);
    }
    PrintList();
}

void Managers::TextureManager::DeleteTexture(const string& name) {
    auto texture = m_textureList.find(name);
    if (texture != m_textureList.end()) {
        // The shared_ptr will be destroyed, which will call Texture destructor
        // and destroy the SDL_Texture
        m_textureList.erase(texture);
        GKC_ENGINE_INFO("Deleted texture: {0}", name);
    }
}

void Managers::TextureManager::DestroyMissingTexture() {
    if (m_missingTexture != nullptr) {
        SDL_DestroyTexture(m_missingTexture);
        m_missingTexture = nullptr;
        GKC_ENGINE_INFO("Missing texture destroyed");
    }
}

shared_ptr<Render::Texture> Managers::TextureManager::GetTextureByName(const string &name) {
    auto texture = m_textureList.find(name);
    if (texture != m_textureList.end()) {
        if(texture->second != nullptr) {
            return texture->second->texture_;
        }
        GKC_ENGINE_WARNING("Texture key exist but hasn't been loaded yet!");
        return nullptr;
    }
    GKC_ENGINE_WARNING("Texture to retrieve doesn't exist!");
    return nullptr;
}

shared_ptr<Render::Texture> Managers::TextureManager::GetTextureByID(TextureID id) {
    auto nameToFind = m_IDToNameList.find(id);
    if (nameToFind != m_IDToNameList.end()) {
        return GetTextureByName(nameToFind->second);
    }
    GKC_ENGINE_WARNING("Texture to retrieve doesn't exist");
    return nullptr;
}

shared_ptr<Render::TextureInfo> Managers::TextureManager::GetTextureInfo(const string &textureName) {
    auto it = m_textureList.find(textureName);
    if(it != m_textureList.end()) {
        if(it->second != nullptr) {
            return it->second;
        }
        GKC_ENGINE_WARNING("Texture Information is NULL!");
        return nullptr;
    }
    GKC_ENGINE_WARNING("Texture Information to retrieve doesn't exist");
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