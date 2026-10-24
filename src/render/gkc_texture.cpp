#include <render/gkc_texture.h>
#include "core/gkc_logger.h"
#include "core/gkc_error.h"
#include "filesys/gkc_filesys.h"

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
    
    m_texture = SDL_CreateTexture(renderer, m_surface->format, 
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

    void* pixels;
    int pitch;
    SDL_LockTexture(m_texture, NULL, &pixels, &pitch);
    SDL_ConvertPixels(m_surface->w, m_surface->h,
        m_surface->format,
        m_surface->pixels, m_surface->pitch,
        m_surface->format,
        pixels, pitch
    );
    
    SDL_UnlockTexture(m_texture);
}

Texture::~Texture() {
    if (m_texture != nullptr) {
        SDL_DestroyTexture(m_texture);
        SDL_DestroySurface(m_surface);
        m_texture = nullptr;
        m_surface = nullptr;
    }
}