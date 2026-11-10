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
#include <core/managers/gkc_manager.h>
#include <render/gkc_animation.h>

namespace Galaktic::Core::Managers {
    /**
     * @class AnimationManager
     * @brief Registers animation assets and controls their playback.
     */
    class AnimationManager : public BaseManager<Render::Animation, Render::AnimationInfo,
        Render::Animation_List, MAX_ANIMATION_QUANTITY, true>
    {
        public:
            /**
             * @brief Initializes this manager using the supplied project context.
             * @param folderPath Folder containing the managed files
             */
            explicit AnimationManager(const std::string& folderPath);
            /**
             * @brief Resumes playback of a named animation asset.
             * @param name Name used to identify the requested entry
             */
            static void PlayAnimation(const std::string& name);
            /**
             * @brief Pauses a named animation asset without resetting its frame.
             * @param name Name used to identify the requested entry
             */
            static void PauseAnimation(const std::string& name);
            /**
             * @brief Stops a named animation asset and resets its frame.
             * @param name Name used to identify the requested entry
             */
            static void StopAnimation(const std::string& name);
            /**
             * @brief Enables looping for the named animation asset.
             * @param name Name used to identify the requested entry
             */
            static void SetLoopToAnimation(const std::string& name);
            /**
             * @brief Advances every loaded animation by the elapsed time in seconds.
             * @param deltaTime Elapsed time in seconds
             */
            static void UpdateAll(float deltaTime);
    };
}