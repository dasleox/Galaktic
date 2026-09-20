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
     * @class AudioFile
     * @brief Stores a \c MIX_Audio and initializes it by providing a path to the sound file
     */
    class AudioFile 
    {
        private:
            MIX_Audio* m_audio = nullptr;
        public:
            /**
             * @param filepath sound filepath
             * @param mixer MIX_Mixer
             */
            explicit AudioFile(const path &filepath, MIX_Mixer* mixer);

            MIX_Audio* GetAudioSample() { return m_audio; }

            /**
             * @brief Checks if the audio initialized is valid for usage
             * @return true if it's valid, false if it is nullptr
             */
            bool IsValid() const
            {
                return m_audio != nullptr;
            }
    };
    
    struct AudioInfo : public AssetInfo<AudioFile>
    {
        using AssetInfo<AudioFile>::AssetInfo;
    };
    
    typedef unordered_map<AudioID, unique_ptr<AudioInfo>> Audio_List;
}