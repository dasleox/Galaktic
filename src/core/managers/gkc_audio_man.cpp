#include <audio/gkc_audio.h>
#include <core/managers/gkc_audio_man.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic;
using namespace Galaktic::Core;

static const vector<string> AUDIO_EXTENSIONS = {".wav", ".ogg", ".mp3", ".flac", ".mod", ".mid", ".mp3"};

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
        if (Filesystem::CheckExtension(file, AUDIO_EXTENSIONS))
            LoadAndRegisterAsset(file, m_mixer);
    }
}

void Managers::AudioManager::PlayAudioFile(const string& name, int loops) {
    if(m_activeTracks.size() >= MAX_TRACKS_QUANTITY)
        return;

    auto* audioInfo = GetAssetInfo(name);
    if (audioInfo != nullptr) {
        auto audioFile = audioInfo->GetAsset()->GetAudioSample();
        auto id = GetAssetIDByName(name);

        if(id == 0)
        {
            Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid audio ID!: {}", name);
            return;
        }
        MIX_Track* track = MIX_CreateTrack(m_mixer);
        MIX_SetTrackAudio(track, audioFile);
        MIX_SetTrackLoops(track, loops);
        MIX_PlayTrack(track, 0);

        m_activeTracks.emplace(id, track);
    }
}

void Managers::AudioManager::PlayMusicFile(const string &name) {
    if(m_activeTracks.size() >= MAX_TRACKS_QUANTITY)
        return;
        
    GKC_ENGINE_INFO("Playing {}", name);
    auto* audioInfo = GetAssetInfo(name);
    if (audioInfo != nullptr) {
        auto audioFile = audioInfo->GetAsset()->GetAudioSample();
        auto id = GetAssetIDByName(name);

        if(id == 0)
        {
            Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid audio ID!: {}", name);
            return;
        }

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
    auto* audioInfo = GetAssetInfo(name);
    if (audioInfo != nullptr) {
        auto id = GetAssetIDByName(name);

        if(id == 0)
        {
            Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid audio ID!: {}", name);
            return;
        }

        auto trackIt = m_activeTracks.find(id);
        if (trackIt != m_activeTracks.end()) {
            MIX_Track* track = trackIt->second;
            if (track != nullptr) {
                MIX_StopTrack(track, fadeOutMs);
                MIX_DestroyTrack(track);
            }
            m_activeTracks.erase(trackIt);
        }
    }
}

void Managers::AudioManager::StopAllTracksFromSound(const string &name, Sint64 fadeOutMs) {
    auto* audioInfo = GetAssetInfo(name);
    if (audioInfo != nullptr) {
        auto id = GetAssetIDByName(name);

        if(id == 0)
        {
            Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid audio ID!: {}", name);
            return;
        }

        auto range = m_activeTracks.equal_range(id);
        for (auto iter = range.first; iter != range.second; ) {
            MIX_Track* track = iter->second;
            if (track != nullptr) {
                MIX_StopTrack(track, fadeOutMs);
                MIX_DestroyTrack(track);
            }
            iter = m_activeTracks.erase(iter);
        }
    }
}

void Managers::AudioManager::StopAllSounds(Sint64 fadeOutMs) {
    MIX_StopAllTracks(m_mixer, fadeOutMs);
    for (auto& [id, track] : m_activeTracks) {
        if (track != nullptr) {
            MIX_DestroyTrack(track);
        }
    }
    m_activeTracks.clear();
}

void Managers::AudioManager::RegisterTrack(AudioID id, MIX_Track *track) {
    m_activeTracks.emplace(id, track);
}
