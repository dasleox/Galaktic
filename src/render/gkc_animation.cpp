#include "render/gkc_animation.h"
#include <core/gkc_logger.h>
#include "filesys/gkc_filesys.h"

using namespace Galaktic::Render;

Galaktic::Render::Animation::Animation(const path& path, SDL_Renderer* renderer) {
    GKC_ENGINE_INFO("Loading {0}...", path.string());
    if (path.empty() || !Filesystem::CheckFile(path)) {
        GKC_ENGINE_ERROR( "given path doesn't exists!");
        return;
    }

    m_animation = IMG_LoadAnimation(path.string().c_str());

    if (m_animation == nullptr) {
        GKC_ENGINE_ERROR("failed to load animaiton!");
        return;
    }

    m_textures = new SDL_Texture*[m_animation->count];
    for (int i = 0; i < m_animation->count; ++i) {
        m_textures[i] = SDL_CreateTextureFromSurface(renderer, m_animation->frames[i]);
        if (m_textures[i] == nullptr) {
            GKC_ENGINE_ERROR("Failed to create texture for frame {0}: {1}", i, SDL_GetError());
        }
    }
}

Galaktic::Render::Animation::~Animation() {
    if (m_textures) {
        for (int i = 0; i < m_animation->count; ++i) {
            if (m_textures[i]) {
                SDL_DestroyTexture(m_textures[i]);
            }
        }
        delete[] m_textures;
        m_textures = nullptr;
    }
    
    if (m_animation) {
        IMG_FreeAnimation(m_animation);
        m_animation = nullptr;
    }
}

void Animation::Update(float deltaTime) {
   if (!IsValid() || !m_isPlaying) return;
    
    m_accumulatedTime += deltaTime;
    
    // Get delay for current frame (IMG_Animation stores delays in milliseconds)
    float frameDelay = static_cast<float>(m_animation->delays[m_currentFrame]) / 1000.0f;
    
    if (m_accumulatedTime >= frameDelay) {
        m_accumulatedTime -= frameDelay;
        m_currentFrame++;
        
        if (m_currentFrame >= m_animation->count) {
            if (m_isLooping) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = m_animation->count - 1;
                m_isPlaying = false;
            }
        }
    }
}

void Animation::Render(SDL_Renderer* renderer, const SDL_FRect& rect) {
    if (!IsValid()) return;
    
    SDL_RenderTexture(renderer, m_textures[m_currentFrame], nullptr, &rect);
}

void Animation::Play() {
    m_isPlaying = true;
}

void Animation::Pause() {
    m_isPlaying = false;
}

void Animation::Stop() {
    m_isPlaying = false;
    Reset();
}

void Animation::Reset() {
    m_currentFrame = 0;
    m_accumulatedTime = 0;
}

void Animation::SetFrame(int frame) {
    if (frame >= 0 && frame < GetFrameCount()) {
        m_currentFrame = frame;
        m_accumulatedTime = 0;
    }
}

bool Galaktic::Render::CheckAnimationExtension(const path& path) {
    if (path.empty() || !Filesystem::CheckFile(path)) {
        return false;
    }
    auto pathStr = path.extension().string();

    if (pathStr == ".gif" || pathStr == ".apng") { return true; }

    return false;
}