/*
 Galaktic Engine
  Copyright (C) 2025 SummerChip

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

typedef Uint32 AudioID;

namespace Galaktic::Audio {
    class AudioFile;
    struct AudioInfo;
    typedef unordered_map<string, unique_ptr<AudioInfo>> Audio_List;
}

namespace Galaktic::Core::Managers {
    class AudioManager {
        public:
            explicit AudioManager(const path& folder);

            void AddAudioFile(const path& filepath);
            void RemoveAudioFile(const string& name);

            void PlayAudioFile(const string& name, int loops = 0);
            void PlayMusicFile(const string& name);
            void StopSound(const string& name, Sint64 fadeOutMs = 0);
            void StopAllSounds(Sint64 fadeOutMs = 0) const;

            Audio::AudioFile* GetAudioFile(const string& name);
            Audio::Audio_List& GetAudioList() { return m_audioFiles; }

            void PrintList() const;
        private:
            Audio::Audio_List m_audioFiles;
            unordered_multimap<AudioID, MIX_Track*> m_activeTracks;
            SDL_AudioSpec m_audioSpec{};
            SDL_AudioDeviceID m_deviceID;
            MIX_Mixer* m_mixer = nullptr;

            void RegisterTrack(AudioID id, MIX_Track* track);
    };
}