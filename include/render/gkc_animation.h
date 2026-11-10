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
    /**
     * @class Animation
     * @brief Owns decoded animation frames and their renderer textures and controls playback.
     */
    class Animation {
        public:
			/**
			 * @brief Loads animation frames and creates their nearest-scaled renderer textures.
			 * @param path Source asset filepath
			 * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
			 */
			Animation(const std::filesystem::path& path, SDL_Renderer* renderer);
            /**
             * @brief Releases the resources owned by this object.
             */
            ~Animation();
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Animation(const Animation&) = delete;
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Animation& operator=(const Animation&) = delete;

            /**
             * @brief Advances animation playback by the elapsed time in seconds, including skipped frames.
             * @param deltaTime Elapsed time in seconds
             */
            void Update(float deltaTime);
            /**
             * @brief Draws the selected animation frame at the destination rectangle and rotation.
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param rect Destination rectangle in renderer coordinates
             * @param rotation Rotation in degrees around the rectangle center
             */
            void Render(SDL_Renderer *renderer, const SDL_FRect &rect, float rotation = 0.f);
            /**
             * @brief Resumes animation playback from the current frame.
             */
            void Play();
            /**
             * @brief Pauses animation playback without resetting its frame.
             */
            void Pause();
            /**
             * @brief Pauses animation playback and resets it to the first frame.
             */
            void Stop();
            /**
             * @brief Selects the first animation frame and clears accumulated frame time.
             */
            void Reset();
            /**
             * @brief Enables or disables animation looping.
             * @param loop Whether playback should repeat after the final frame
             */
            void SetLoop(bool loop) { m_isLooping = loop; }
            /**
             * @brief Selects a valid zero-based frame and resets elapsed frame time; ignores an invalid index.
             * @param frame Zero-based animation frame index
             */
            void SetFrame(int frame);

            /**
             * @brief Returns the borrowed decoded animation and its CPU frame surfaces.
             */
            [[nodiscard]] IMG_Animation* GetAnimation() { return m_animation; }
            /**
             * @brief Returns the width in pixels.
             */
			int GetWidth() const { return m_animation ? m_animation->w : 0; }
            /**
             * @brief Returns the height in pixels.
             */
            int GetHeight() const { return m_animation ? m_animation->h : 0; }
            /**
             * @brief Returns the number of decoded frames, or zero for an unloaded animation.
             */
            int GetFrameCount() const { return m_animation ? m_animation->count : 0; }
            /**
             * @brief Returns the zero-based animation frame selected for rendering.
             */
            int GetCurrentFrame() const { return m_currentFrame; }
            /**
             * @brief Returns the animation's playback-enabled flag.
             * @return true on success, false otherwise
             */
            bool IsPlaying() const { return m_isPlaying; }
            /**
             * @brief Checks whether animation playback wraps after the final frame.
             * @return true if animation playback wraps after the final frame, false otherwise
             */
            bool IsLooping() const { return m_isLooping; }
            /**
             * @brief Checks whether a valid, non-looping animation has stopped at its final frame.
             * @return true if a valid, non-looping animation has stopped at its final frame, false otherwise
             */
            bool IsFinished() const { return IsValid() && !m_isLooping && !m_isPlaying && m_currentFrame == GetFrameCount() - 1; }
            /**
             * @brief Checks whether this handle or underlying resource is initialized for use.
             * @return true if this handle or underlying resource is initialized for use, false otherwise
             */
            bool IsValid() const { return m_animation != nullptr && m_textures != nullptr; }
		private:
			IMG_Animation* m_animation = nullptr;
            SDL_Texture** m_textures = nullptr;
            
            int m_currentFrame = 0;
            double m_accumulatedTime = 0.0;
            bool m_isPlaying = true;
            bool m_isLooping = true;
    };

    /**
     * @struct AnimationInfo
     * @brief Stores metadata and unique ownership of an animation asset.
     */
    struct AnimationInfo : public AssetInfo<Animation>
    {
        using AssetInfo<Animation>::AssetInfo;
    };

    typedef std::unordered_map<uint32_t, std::unique_ptr<Render::AnimationInfo>> Animation_List;
}
