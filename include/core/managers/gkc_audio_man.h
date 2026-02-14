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
    class AudioFile;
    struct AudioInfo;
    typedef unordered_map<string, shared_ptr<AudioInfo>> Audio_List;
}

namespace Galaktic::Core::Managers {
    /**
     * @class AudioManager
     *
     * Manages audio files and current tracks of the audio files, tracks
     * can be stopped, played or paused, this class automatically sets the audio device
     * to play audio in the following configuration: \n (default: 2 channels,
     * 48000Hz, \c SDL_AUDIO_S16 formatting)
     *
     * An \c MIX_Mixer pointer is created using the previously created device
     * to play tracks. \n
     * All tracks are saved into a \c std::multimap, the key is the \c AudioID from an
     * \c AudioInfo instance inside the audio list, many tracks with the same sound
     * can be played, if a track is stopped using \c StopSound the first track played
     * will be stopped inside the multimap. \n Use \c StopAllTracksFromSound to stop all
     * tracks playing the same sound
     *
     * @attention The key values for the audio list (m_audioFiles) are registered
     * with their full name, <b> (extension included!) </b> don't forget to add the
     * appropriate extension when playing, stopping or pausing tracks!
     */
    class AudioManager {
        public:
            /**
             * @param folder folder path (audio folder)
             */
            explicit AudioManager(const string& folder);

            /**
             * Adds an audio file to the list, an ID for the file
             * its assigned automatically,
             * @param filepath audio filepath
             */
            static void AddAudioFile(const string& filepath);

            /**
             * Removes an audio file from the list, all tracks referring
             * to the file will be stopped
             * @param name audio filename
             */
            static void RemoveAudioFile(const string& name);

            /**
             * Plays a track from an audio file depending on the times
             * of loops passed <b>(passing -1, will play the sound forever until
             * stopped or dereferenced)</b>, the audio file should exist in the
             * list in order to play
             * @param name audio filename
             * @param loops times to loop
             */
            static void PlayAudioFile(const string& name, int loops = 0);

            /**
             * Plays a track indefinitely until stopped or dereferenced
             * , the audio file should exist in the list in order to play
             * @param name music filename
             */
            static void PlayMusicFile(const string& name);

            /**
             * Stops a track using the audio file's name, a fade time in
             * milliseconds can be added as a parameter.
             * @param name audio filename
             * @param fadeOutMs fade ms
             *
             * @note Only the first track played will be stopped, if there are
             * more tracks you would like to stop use \c StopAllTracksFromSound
             */
            static void StopSound(const string& name, Sint64 fadeOutMs = 0);

            /**
             * Stops all tracks related to the audio file's name, a fade time in
             * milliseconds can be added as a parameter
             * @param name audio filename
             * @param fadeOutMs fade ms
             */
            static void StopAllTracksFromSound(const string& name, Sint64 fadeOutMs = 0);

            /**
             * Stops \b ALL tracks inside the audio list, a fade time in
             * milliseconds can be added as parameter
             * @param fadeOutMs fade ms
             */
            static void StopAllSounds(Sint64 fadeOutMs = 0);

            /**
             * @brief Returns a pointer of an audio file inside the list
             *        using the associated key, returns nullptr otherwise.
             *        A warning is emitted that file doesn't exist
             * @param name audio filename
             * @return A pointer to the audio file, nullptr if no file was found
             */
            static shared_ptr<Audio::AudioFile> GetAudioFile(const string& name);

            static shared_ptr<Audio::AudioFile> GetAudioFile(AudioID id);

            static shared_ptr<Audio::AudioInfo> GetAudioInfo(const string& name);

            static Audio::Audio_List& GetAudioList() { return m_audioFiles; }

            /**
             * Prints the list of all audio files with their filenames,
             * ID's and address in memory
             */
            static void PrintList();
        private:
            static Audio::Audio_List m_audioFiles;
            static std::multimap<AudioID, MIX_Track*> m_activeTracks;
            static SDL_AudioSpec m_audioSpec;
            static SDL_AudioDeviceID m_deviceID;
            static MIX_Mixer* m_mixer;

            /**
             * Helper to register a track inside the track list
             * @param id AudioInfo's id
             * @param track Track created from an AudioFile
             */
            static void RegisterTrack(AudioID id, MIX_Track* track);
    };
}