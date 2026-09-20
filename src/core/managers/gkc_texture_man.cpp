#include <core/managers/gkc_texture_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"
#include "render/gkc_texture.h"
#include "ecs/gkc_entity.h"
#include "core/managers/gkc_texture_man.h"

using namespace Galaktic::Core;
using namespace Galaktic;

SDL_Texture* Managers::TextureManager::m_missingTexture = nullptr;

static const vector<string> TEXTURE_EXTENSIONS = {".png", ".jpg", ".jpeg", ".webp", ".bmp", ".gif"};

Managers::TextureManager::TextureManager(const string &path) {
    auto files = Filesystem::GetFilenamesInFolder(path);
    for (auto& file : files) {
        if (Filesystem::CheckExtension(file, TEXTURE_EXTENSIONS))
            RegisterAsset(file);
    }
}

void Managers::TextureManager::DestroyMissingTexture() {
    if (m_missingTexture != nullptr) {
        SDL_DestroyTexture(m_missingTexture);
        m_missingTexture = nullptr;
        GKC_ENGINE_INFO("Missing texture destroyed");
    }
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
    SDL_DestroySurface(surface);

    GKC_RELEASE_ASSERT(m_missingTexture != nullptr, "Even the missing texture failed to be created, you are fucked :)");
}