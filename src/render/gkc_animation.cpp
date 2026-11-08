#include "render/gkc_animation.h"
#include <algorithm>
#include <cmath>
#include <core/gkc_logger.h>
#include "filesys/gkc_filesys.h"
#include "core/gkc_error.h"

using namespace Galaktic::Render;

Galaktic::Render::Animation::Animation(const path& path, SDL_Renderer* renderer) {
    m_animation = IMG_LoadAnimation(path.string().c_str());

    if (m_animation == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError, "failed to load animaiton!");
        return;
    }

    if (m_animation->count <= 0 || !m_animation->frames || !m_animation->delays) {
        IMG_FreeAnimation(m_animation);
        m_animation = nullptr;
        return;
    }
    m_textures = new SDL_Texture*[m_animation->count]{};
    for (int i = 0; i < m_animation->count; ++i) {
        m_textures[i] = SDL_CreateTextureFromSurface(renderer, m_animation->frames[i]);
        if (m_textures[i] != nullptr) {
            SDL_SetTextureScaleMode(m_textures[i], SDL_SCALEMODE_NEAREST);
        }
        if (m_textures[i] == nullptr) {
            Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError, "Failed to create texture for frame {0}: {1}", i, SDL_GetError());
            for (int j = 0; j < i; ++j) SDL_DestroyTexture(m_textures[j]);
            delete[] m_textures;
            m_textures = nullptr;
            IMG_FreeAnimation(m_animation);
            m_animation = nullptr;
            return;
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
   if (!IsValid() || !m_isPlaying || !std::isfinite(deltaTime) || deltaTime < 0.f) return;
    
    m_accumulatedTime += deltaTime;
    
    // Skip complete cycles without iterating once for every elapsed frame.
    if (m_isLooping) {
        double cycle = 0.0;
        for (int i = 0; i < m_animation->count; ++i)
            cycle += std::max(1, m_animation->delays[i]) / 1000.0;
        m_accumulatedTime = std::fmod(m_accumulatedTime, cycle);
    }
    while (m_isPlaying) {
        const double frameDelay = std::max(1, m_animation->delays[m_currentFrame]) / 1000.0;
        if (m_accumulatedTime < frameDelay) break;
        m_accumulatedTime -= frameDelay;
        m_currentFrame++;
        
        if (m_currentFrame >= m_animation->count) {
            if (m_isLooping) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = m_animation->count - 1;
                m_isPlaying = false;
                m_accumulatedTime = 0.0;
            }
        }
    }
}

void Animation::Render(SDL_Renderer* renderer, const SDL_FRect& rect, float rotation) {
    if (!IsValid()) return;
    
    SDL_RenderTextureRotated(renderer, m_textures[m_currentFrame], nullptr, &rect, rotation, nullptr, SDL_FLIP_NONE);
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
