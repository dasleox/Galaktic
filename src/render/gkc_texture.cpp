#include <render/gkc_texture.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic::Render;

Texture::Texture(const path &path, SDL_Renderer* renderer) {
    GKC_ENGINE_INFO("Loading {0}...", path.string());
    if (path.empty() || !Filesystem::CheckFile(path)) {
        GKC_ENGINE_ERROR( "given path doesn't exists!");
        return;
    }

    m_surface = IMG_Load(path.string().c_str());
    if(m_surface == nullptr) {
        GKC_ENGINE_ERROR("Failed to create m_surface for texture: {}", Filesystem::GetFilename(path));
        return;
    }
    
    m_texture = SDL_CreateTexture(renderer, m_surface->format, 
        SDL_TEXTUREACCESS_STREAMING,
        m_surface->w, m_surface->h
    );
    
    if (m_texture == nullptr) {
        GKC_ENGINE_ERROR("failed to load texture!");
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
        m_texture = nullptr;
    }
}