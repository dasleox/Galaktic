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

namespace Galaktic::Audio 
{
    /**
     * @class AudioGroup
     * @brief Groups audio asset IDs and applies a shared volume limit to their active tracks.
     */
    class AudioGroup 
    {  
        private:
            float m_maxVolume = 1.f;
            float m_currentVolume = 1.f;
            std::set<AudioID> m_audioList;
        public:
            /**
             * @brief Initializes an audio group with its volume limit and current gain.
             */
            AudioGroup() = default;
            /**
             * @brief Initializes an audio group with its volume limit and current gain.
             * @param maxVolume Maximum allowed audio gain
             */
            explicit AudioGroup(float maxVolume);

            /**
             * @brief Sets the maximum allowed volume.
             * @param volume Requested audio gain in the range [0, 1]
             */
            void SetMaxVolume(float volume);
            /**
             * @brief Clamps and applies the requested group volume to all member sounds' active tracks.
             * @param volume Requested audio gain in the range [0, 1]
             */
            void SetVolume(float volume);
            /**
             * @brief Sets the group's stored and active-track volume to zero.
             */
            void Mute();
            /**
             * @brief Adds an existing audio asset ID to this group.
             * @param id Entity or asset ID used by this operation
             */
            void AddAudioFromGroup(AudioID id);
            /**
             * @brief Removes an audio asset ID from this group, reporting an absent member.
             * @param id Entity or asset ID used by this operation
             */
            void DeleteAudioFromGroup(AudioID id);

            /**
             * @brief Returns the configured volume limit.
             */
            float GetMaxVolume() const { return m_maxVolume; }
            /**
             * @brief Returns the group's most recently applied volume.
             */
            float GetCurrentVolume() const { return m_currentVolume; }
            /**
             * @brief Returns the read-only audio asset IDs belonging to this group.
             */
            const std::set<AudioID>& GetAudioList() const { return m_audioList; }
    };
}
