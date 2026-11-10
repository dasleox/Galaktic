#include <render/gkc_texture.h>
#include "core/gkc_logger.h"
#include "core/gkc_error.h"
#include "filesys/gkc_filesys.h"

using std::filesystem::path;

using namespace Galaktic::Render;

Texture::Texture(const path &path, SDL_Renderer* renderer) {
    GKC_ENGINE_INFO("Loading {0}...", path.string());
    if (path.empty() || !Filesystem::CheckFile(path)) {
        Debug::Logger::LogErrorWithType(ErrorType::FileNotFound, "given path doesn't exists!");
        return;
    }

    m_surface = IMG_Load(path.string().c_str());
    if(m_surface == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError,
            "Failed to create m_surface for texture: {}", Filesystem::GetFilename(path));
        return;
    }
    
    // Pixel groups address four RGBA bytes per pixel. Normalize indexed/RGB
    // images once at load time instead of writing past their pixel buffers.
    if (m_surface->format != SDL_PIXELFORMAT_RGBA32) {
        SDL_Surface* converted = SDL_ConvertSurface(m_surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(m_surface);
        m_surface = converted;
        if (m_surface == nullptr) return;
    }

    m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        m_surface->w, m_surface->h
    );
    
    if (m_texture != nullptr) {
        SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST);
    }

    if (m_texture == nullptr) {
        Debug::Logger::LogFatalWithType(ErrorType::AssetLoadError, "failed to load texture!");
        return;
    }

    if (!SDL_UpdateTexture(m_texture, nullptr, m_surface->pixels, m_surface->pitch)) {
        Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError, "Failed to upload texture: {}", SDL_GetError());
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

Texture::~Texture() {
    SDL_DestroyTexture(m_texture);
    SDL_DestroySurface(m_surface);
}
