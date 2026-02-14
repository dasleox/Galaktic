#include <audio/gkc_audio.h>
#include <core/managers/gkc_audio_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic;
using namespace Galaktic::Core;

Galaktic::Audio::Audio_List Managers::AudioManager::m_audioFiles;
std::multimap<AudioID, MIX_Track*> Managers::AudioManager::m_activeTracks;
SDL_AudioSpec Managers::AudioManager::m_audioSpec{};
SDL_AudioDeviceID Managers::AudioManager::m_deviceID;
MIX_Mixer* Managers::AudioManager::m_mixer = nullptr;

Managers::AudioManager::AudioManager(const string &folder) {
    m_audioSpec.channels = 2;
    m_audioSpec.freq = 48000;
    m_audioSpec.format = SDL_AUDIO_S16;

    m_deviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec);
    SDL_PauseAudioDevice(m_deviceID);

    if (m_deviceID == 0) {
        GKC_ENGINE_FATAL("failed to open audio device!");
    }

    m_mixer = MIX_CreateMixerDevice(m_deviceID, &m_audioSpec);
    if (m_mixer == nullptr) {
        GKC_ENGINE_FATAL("audio mixer is NULL!");
    }


    auto files = Filesystem::GetFilenamesInFolder(folder);
    for (auto& file : files) {
        if (Audio::CheckAudioExtension(file))
            AddAudioFile(file);
    }
}

void Managers::AudioManager::AddAudioFile(const string& path) {
    AudioID id = m_audioFiles.size() + 1;
    auto info = make_shared<Audio::AudioInfo>(id, make_shared<Audio::AudioFile>(path, m_mixer));

    if(!info->audioFile_->IsValid() || info->id_ == 0) {
        GKC_ENGINE_ERROR("Audio file is invalid!: {}", path);
        return;
    }

    auto [it, inserted] = m_audioFiles.emplace(Filesystem::GetFilename(path),
        std::move(info));

    #ifdef GKC_PRINT_TEXTURE_ADDED
        GKC_ENGINE_INFO("[{0} | ID: {1}] at address {2}", path, id,
            CastToVoidPtr(it->second->audioFile_));
    #endif
}

void Managers::AudioManager::RemoveAudioFile(const string &name) {
    auto audioFile = m_audioFiles.find(name);
    if (audioFile != m_audioFiles.end()) {
        StopAllTracksFromSound(name);
        m_audioFiles.erase(audioFile);
        GKC_ENGINE_INFO("Erased audio file {0}", name);
    }
}

void Managers::AudioManager::PlayAudioFile(const string& name, int loops) {
    auto it = m_audioFiles.find(name);
    if (it != m_audioFiles.end()) {
        auto audioFile = it->second->audioFile_->GetAudioSample();
        auto id = it->second->id_;

        MIX_Track* track = MIX_CreateTrack(m_mixer);
        MIX_SetTrackAudio(track, audioFile);
        MIX_SetTrackLoops(track, loops);
        MIX_PlayTrack(track, 0);

        m_activeTracks.emplace(id, track);
    }
}

void Managers::AudioManager::PlayMusicFile(const string &name) {
    GKC_ENGINE_INFO("Playing {}", name);
    auto it = m_audioFiles.find(name);
    if (it != m_audioFiles.end()) {
        auto audioFile = it->second->audioFile_->GetAudioSample();
        auto id = it->second->id_;

        MIX_Track* track = MIX_CreateTrack(m_mixer);
        MIX_SetTrackAudio(track, audioFile);

        MIX_PlayTrack(track, 0);
        MIX_SetTrackLoops(track, -1);

        m_activeTracks.emplace(id, track);
    } else {
        GKC_ENGINE_ERROR("file doesn't exist!");
    }
}

void Managers::AudioManager::StopSound(const string &name, Sint64 fadeOutMs) {
    auto it = m_audioFiles.find(name);
    if (it != m_audioFiles.end()) {
        auto id = it->second->id_;

        auto trackIt = m_activeTracks.find(id);
        if (trackIt != m_activeTracks.end()) {
            MIX_Track* track = trackIt->second;
            if (track != nullptr) {
                MIX_StopTrack(track, fadeOutMs);
                // CRITICAL FIX: Destroy the track to free memory
                MIX_DestroyTrack(track);
            }
            m_activeTracks.erase(trackIt);
        }
    }
}

void Managers::AudioManager::StopAllTracksFromSound(const string &name, Sint64 fadeOutMs) {
    auto it = m_audioFiles.find(name);
    if (it != m_audioFiles.end()) {
        auto id = it->second->id_;

        auto range = m_activeTracks.equal_range(id);
        for (auto iter = range.first; iter != range.second; ) {
            MIX_Track* track = iter->second;
            if (track != nullptr) {
                MIX_StopTrack(track, fadeOutMs);
                // CRITICAL FIX: Destroy the track to free memory
                MIX_DestroyTrack(track);
            }
            iter = m_activeTracks.erase(iter);
        }
    }
}

void Managers::AudioManager::StopAllSounds(Sint64 fadeOutMs) {
    MIX_StopAllTracks(m_mixer, fadeOutMs);
    // CRITICAL FIX: Destroy all tracks and clear the map
    for (auto& [id, track] : m_activeTracks) {
        if (track != nullptr) {
            MIX_DestroyTrack(track);
        }
    }
    m_activeTracks.clear();
}

shared_ptr<Audio::AudioFile> Managers::AudioManager::GetAudioFile(const string &name) {
    auto audioFile = m_audioFiles.find(name);
    if (audioFile != m_audioFiles.end()) {
        return audioFile->second->audioFile_;
    }
    GKC_ENGINE_WARNING("Audio file to retrieve doesn't exist");
    return nullptr;
}

void Managers::AudioManager::PrintList() {
    for (auto& pair : m_audioFiles) {
        GKC_ENGINE_INFO("Key: {0} | {1} &{2}", pair.first, pair.second->id_,
            CastToVoidPtr(pair.second->audioFile_.operator*()));
    }
}

void Managers::AudioManager::RegisterTrack(AudioID id, MIX_Track *track) {
    m_activeTracks.emplace(id, track);
}
