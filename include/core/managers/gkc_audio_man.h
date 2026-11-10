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
#include <audio/gkc_audio.h>

namespace Galaktic::Audio
{
    class AudioGroup;
}

namespace Galaktic::Core::Managers {
    /**
     * @class AudioManager
     *
     * Manages audio files and current tracks of the audio files, tracks
     * can be stopped, played or paused, this class automatically sets the audio device
     * to play audio in the following default configuration: \n (default: 2 channels,
     * 48000Hz, \c SDL_AUDIO_F32 formatting), however you can modify this configuration however
     * you want.
     *
     * An \c MIX_Mixer pointer is created using the previously created device
     * to play tracks. \n
     * All tracks are saved into a \c std::multimap, the key is the \c AudioID from an
     * \c AudioInfo instance inside the audio list, many tracks with the same sound
     * can be played, if a track is stopped using \c StopSound the first track played
     * will be stopped inside the multimap. \n Use \c StopAllTracksFromSound to stop all
     * tracks playing the same sound
     * 
     * Audios can be grouped with \c CreateAudioGroup which adds an AudioGroup instance to
     * the audio group list, this can be useful to group audios to mute specific type of sounds
     * in your game, for example with this you can mute the SFX sounds, Music, etc.
     * 
     * @attention The key values for the audio list (m_audioFiles) are registered
     * with their full name, <b> (extension included!) </b> don't forget to add the
     * appropriate extension when playing, stopping or pausing tracks!
     */
    class AudioManager : public BaseManager<Audio::AudioFile, Audio::AudioInfo, Audio::Audio_List,
        MAX_SOUNDFILE_QUANTITY, true>
    {
        public:
            /**
             * @param folder folder path (audio folder)
             */
            explicit AudioManager(const std::string& folder);

            /**
             * Plays a track from an audio file depending on the times
             * of loops passed <b>(passing -1, will play the sound forever until
             * stopped or dereferenced)</b>, the audio file should exist in the
             * list in order to play
             * @param name audio filename
             * @param loops times to loop
             */
            static void PlayAudioFile(const std::string& name, int loops = 0);

            /**
             * Plays a track indefinitely until stopped or dereferenced
             * , the audio file should exist in the list in order to play
             * @param name music filename
             */
            static void PlayMusicFile(const std::string& name);

            /**
             * Stops a track using the audio file's name, a fade time in
             * milliseconds can be added as a parameter.
             * @param name audio filename
             * @param fadeOutMs fade ms
             *
             * @note Only the first track played will be stopped, if there are
             * more tracks you would like to stop use \c StopAllTracksFromSound
             */
            static void StopSound(const std::string& name, Sint64 fadeOutMs = 0);

            /**
             * Stops all tracks related to the audio file's name, a fade time in
             * milliseconds can be added as a parameter
             * @param name audio filename
             * @param fadeOutMs fade ms
             */
            static void StopAllTracksFromSound(const std::string& name, Sint64 fadeOutMs = 0);

            /**
             * Stops \b ALL tracks inside the audio list, a fade time in
             * milliseconds can be added as parameter
             * @param fadeOutMs fade ms
             */
            static void StopAllSounds(Sint64 fadeOutMs = 0);


            /**
             * @brief Reloads changed audio assets when their tracks are not playing.
             */
            static void ReloadAudioFiles();

            /**
             * @brief Sets the volume for all the scene.   
             * 
             * @param volume Volume
             */
            static void SetMasterVolume(float volume);

            /**
             * Checks if a track is currently active based on its ID
             * @param id AudioInfo's id
             * @return true if the track is active, false otherwise
             */
            static bool IsTrackActive(AudioID id);

            /**
             * @brief Set the volume for all the tracks with the id.
             * 
             * @param id AudioInfo ID
             * @param volume Volume
             */
            static void SetVolumeByID(AudioID id, float volume);
        
            /**
             * @brief Clamps a gain value to the supported range [0, 1].
             * @param volume Requested audio gain in the range [0, 1]
             */
            static float ClampVolume(float volume) {
                return std::clamp(volume, 0.f, 1.f);
            }

            /**
             * @brief Set the quantity of channels to use in the audio device
             * Audio channel configurations:
             * Mono (1): Single audio channel. \n 
             * Stereo (2): Two channels, left and right. \n
             * Stereo 2.1 (3): Two main channels (left and right) + one subwoofer channel. \n
             * Quad (4): Four channels for quadraphonic audio (front left/right, rear left/right). \n
             * Quad 4.1 (5): Four main channels + one subwoofer channel. \n
             * Surround 5.1 (6): Five main channels + one subwoofer channel. \n
             * Surround 6.1 (7): Six main channels + one subwoofer channel. \n
             * Surround 7.1 (8): Seven main channels + one subwoofer channe \n
             * @see To understand more about channels read SDL_mixer.h
             * @param channels 
             */
            static void SetAudioChannels(int channels);

            /**
             * @brief Updates the requested audio frequency and restarts the audio device.
             * @param frequency Frequency
             */
            static void SetAudioFrequency(int frequency);
            /**
             * @brief Updates the requested audio format and restarts the audio device.
             * @param format Format
             */
            static void SetAudioFormat(SDL_AudioFormat format);

            /**
             * @brief Updates the requested audio format and restarts the audio device.
             * @param format Format
             */
            static void SetAudioFormat(std::string format);

            /**
             * @brief Creates a named group with the requested volume limit.
             * @param name Name used to identify the requested entry
             * @param maxVolume Maximum allowed audio gain
             */
            static void CreateAudioGroup(const std::string& name, float maxVolume);
            /**
             * @brief Applies volume to the active tracks of a named audio group.
             * @param name Name used to identify the requested entry
             * @param volume Requested audio gain in the range [0, 1]
             */
            static void SetAudioVolumeByGroup(const std::string& name, float volume);
            /**
             * @brief Removes a named audio group.
             * @param name Name used to identify the requested entry
             */
            static void DeleteAudioGroup(const std::string& name);
            /**
             * @brief Returns a copy of the named audio group, or a default group when absent.
             * @param name Name used to identify the requested entry
             */
            static Audio::AudioGroup GetAudioGroupByName(const std::string& name);

            /**
             * @brief Sets the maximum allowed volume.
             * @param volume Requested audio gain in the range [0, 1]
             */
            static void SetMaxVolume(float volume) { m_maxVolume = volume; }
            /**
             * @brief Returns the configured volume limit.
             */
            static float GetMaxVolume() { return m_maxVolume; }
        private:    
            static std::multimap<AudioID, MIX_Track*> m_activeTracks;
            static SDL_AudioSpec m_audioSpec;
            static SDL_AudioDeviceID m_deviceID;
            static MIX_Mixer* m_mixer;
            static std::unordered_map<std::string, Audio::AudioGroup> m_audioGroupList;
            static float m_maxVolume;

            /**
             * Helper to register a track inside the track list
             * @param id AudioInfo's id
             * @param track Track created from an AudioFile
             */
            static void RegisterTrack(AudioID id, MIX_Track* track);

            /**
             * @brief Stops tracks and recreates the mixer using the current audio specification.
             */
            static void RestartAudioDevice();

            /**
             * Applies the group's current volume to every track playing
             * a sound that belongs to the group
             * @param group Audio group
             */
            static void ApplyGroupVolume(const Audio::AudioGroup& group);
    };
}