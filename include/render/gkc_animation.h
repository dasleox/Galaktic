/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#include <pch.hpp>

namespace Galaktic::Render {
    class Animation {
        public:
			Animation(const path& path, SDL_Renderer* renderer);
            ~Animation();

            void Update(float deltaTime);
            void Render(SDL_Renderer *renderer, const SDL_FRect &rect);
            void Play();
            void Pause();
            void Stop();
            void Reset();
            void SetLoop(bool loop) { m_isLooping = loop; }
            void SetFrame(int frame);

            [[nodiscard]] IMG_Animation* GetAnimation() { return m_animation; }
			int GetWidth() const { return m_animation ? m_animation->w : 0; }
            int GetHeight() const { return m_animation ? m_animation->h : 0; }
            int GetFrameCount() const { return m_animation ? m_animation->count : 0; }
            int GetCurrentFrame() const { return m_currentFrame; }
            bool IsPlaying() const { return m_isPlaying; }
            bool IsLooping() const { return m_isLooping; }
            bool IsFinished() const { return !m_isLooping && m_currentFrame >= GetFrameCount() - 1; }
            bool IsValid() const { return m_animation != nullptr && m_textures != nullptr; }
		private:
			IMG_Animation* m_animation = nullptr;
            SDL_Texture** m_textures = nullptr;
            
            int m_currentFrame = 0;
            float m_accumulatedTime = 0;
            bool m_isPlaying = true;
            bool m_isLooping = true;
    };

    struct AnimationInfo {
        AnimationID id_;
        shared_ptr<Animation> animation_;
    };

    typedef unordered_map<string, shared_ptr<Render::AnimationInfo>> Animation_List;
    typedef unordered_map<AnimationID, string> AnimationID_List;

    bool CheckAnimationExtension(const path& path);
}