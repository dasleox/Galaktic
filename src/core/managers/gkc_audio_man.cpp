#include <audio/gkc_audio.h>
#include <core/managers/gkc_audio_man.h>

#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic;
using namespace Galaktic::Core;

//@todo Refactor this system so it can be more customizable

Managers::AudioManager::AudioManager(const path &folder) {
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

void Managers::AudioManager::AddAudioFile(const path& path) {
    AudioID id = m_audioFiles.size() + 1;
    auto info = make_unique<Audio::AudioInfo>(id, make_unique<Audio::AudioFile>(path, m_mixer));
    auto [it, inserted] = m_audioFiles.emplace(Filesystem::GetFilename(path),
        std::move(info));

    #ifdef GKC_PRINT_TEXTURE_ADDED
        GKC_ENGINE_INFO("[{0} | ID: {1}] at address {2}", path.string(), id,
            CastToVoidPtr(it->second->audioFile_.operator*()));
    #endif
}

void Managers::AudioManager::RemoveAudioFile(const string &name) {
    auto audioFile = m_audioFiles.find(name);
    if (audioFile != m_audioFiles.end()) {
        m_audioFiles.erase(audioFile);
        GKC_ENGINE_INFO("Erased {0}", name);
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

        auto track = m_activeTracks.find(id)->second;
        if (track != nullptr) {
            MIX_StopTrack(track, fadeOutMs);
        }
    }
}

void Managers::AudioManager::StopAllSounds(Sint64 fadeOutMs) const {
    MIX_StopAllTracks(m_mixer, fadeOutMs);
}

Audio::AudioFile* Managers::AudioManager::GetAudioFile(const string &name) {
    auto audioFile = m_audioFiles.find(name);
    if (audioFile != m_audioFiles.end()) {
        return audioFile->second->audioFile_.get();
    }
    GKC_ENGINE_WARNING("Audio file doesn't exist");
    return nullptr;
}

void Managers::AudioManager::PrintList() const {
    for (auto& pair : m_audioFiles) {
        GKC_ENGINE_INFO("Key: {0} | {1} &{2}", pair.first, pair.second->id_,
            CastToVoidPtr(pair.second->audioFile_.operator*()));
    }
}

void Managers::AudioManager::RegisterTrack(AudioID id, MIX_Track *track) {
    m_activeTracks.emplace(id, track);
}
