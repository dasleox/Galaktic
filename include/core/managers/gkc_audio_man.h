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

    ///@todo find use for this map :v
    /** I dont know where to use it yet ¯\_(ツ)_/¯
    const inline std::map<string, int> ChannelTypeMap {
        {"Mono", 1}, {"Stereo", 2}, {"Stereo 2.1", 3},
        {"Quad", 4}, {"Quad 4.1", 5}, {"Surround 5.1", 6},
        {"Surround 6.1" , 7}, {"Inmersive 7.1", 8}
    };
    */
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
            using AudioGroup_List = std::unordered_map<std::string, Audio::AudioGroup>;

            /**
             * @param folder folder path (audio folder)
             */
            explicit AudioManager(const string& folder);

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
             * @brief Reloads all audio files based on their last time that the file's was written.
             *        and checks if the file is being used/played, if the track is being played
             *        the function waits until the track is stopped in order to reload it.
             * 
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
        
            static inline float ClampVolume(float Volume);

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

            static void SetAudioFrequency(int frequency);
            static void SetAudioFormat(SDL_AudioFormat format);

            static void CreateAudioGroup(const string& name, float maxVolume);
            static void SetAudioVolumeByGroup(const string& name, float volume);
            static void DeleteAudioGroup(const string& name);
            static Audio::AudioGroup GetAudioGroupByName(const string& name);

            static void SetMaxVolume(float volume) { m_maxVolume = volume; }
            static float GetMaxVolume() { return m_maxVolume; }
        private:    
            static std::multimap<AudioID, MIX_Track*> m_activeTracks;
            static SDL_AudioSpec m_audioSpec;
            static SDL_AudioDeviceID m_deviceID;
            static MIX_Mixer* m_mixer;
            static AudioGroup_List m_audioGroupList; 

            static float m_maxVolume;

            /**
             * Helper to register a track inside the track list
             * @param id AudioInfo's id
             * @param track Track created from an AudioFile
             */
            static void RegisterTrack(AudioID id, MIX_Track* track);

            static bool AudioGroupExists(const string& name);
    };
}