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

namespace Galaktic::Audio {
    /**
     * @class AudioFile
     * @brief Stores a \c MIX_Audio and initializes it by providing a path to the sound file
     */
    class AudioFile {
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
            bool IsValid() const;
        private:
            MIX_Audio* m_audio = nullptr;
    };

    /**
     * @struct AudioInfo
     * @brief Wraps an \c AudioFile instance with an AudioID
     *
     * This struct is used to keep track of audio files and play them by ID,
     * if the ID is 0, it means the audio file is invalid, and it will not play
     * or even cause an error
     */
    struct AudioInfo {
        AudioID id_;
        unique_ptr<AudioFile> audioFile_;
    };

    /**
     * @brief Type for saving the filename <b>(includes extension, e.g: file.mp3) </b> and
     *        a unique_ptr to an \c AudioInfo struct
     */
    typedef unordered_map<string, unique_ptr<AudioInfo>> Audio_List;

    /**
     * @brief Checks if the file is an audio file
     * @param path filepath
     * @return true if the file is an audio file, false otherwise
     */
    extern bool CheckAudioExtension(const path& path);
}

/**
 * @brief Gets the audio file from an \c AudioFile instance
 * @param file audio file
 */
#define GKC_GET_AUDIOFILE(file) file->GetAudioSample();