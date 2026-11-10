#include <audio/gkc_audio.h>
#include <core/managers/gkc_audio_man.h>
#include "core/gkc_logger.h"
#include <config/gkc_config_setter.h>
#include "filesys/gkc_filesys.h"
#include <audio/gkc_audio_group.h>

using std::string;
using std::vector;

using namespace Galaktic;
using namespace Galaktic::Core;

static const vector<string> AUDIO_EXTENSIONS = {".wav", ".ogg", ".mp3", ".flac", ".mod", ".mid", ".mp3"};

std::multimap<AudioID, MIX_Track*> Managers::AudioManager::m_activeTracks;
SDL_AudioSpec Managers::AudioManager::m_audioSpec{};
SDL_AudioDeviceID Managers::AudioManager::m_deviceID;
MIX_Mixer* Managers::AudioManager::m_mixer = nullptr;
std::unordered_map<string, Audio::AudioGroup> Managers::AudioManager::m_audioGroupList;
float Managers::AudioManager::m_maxVolume = 1.f;

Managers::AudioManager::AudioManager(const string &folder) {
    m_audioSpec.channels = 2;
    m_audioSpec.freq = 48000;
    m_audioSpec.format = SDL_AUDIO_F32;

    Config::ConfigSetter::SetAudioConfigurationFromFile();

    m_deviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec);
    SDL_PauseAudioDevice(m_deviceID);

    if (m_deviceID == 0) {
        Debug::Logger::LogFatalWithType(ErrorType::DeviceNotFound,"failed to open audio device!");
    }

    m_mixer = MIX_CreateMixerDevice(m_deviceID, &m_audioSpec);
    if (m_mixer == nullptr) {
        Debug::Logger::LogFatalWithType(ErrorType::NullPointer, "audio mixer is NULL!");
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
        Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound, "File '{}' doesn't exist!", name);
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

bool Managers::AudioManager::IsTrackActive(AudioID id) {
    const auto range = m_activeTracks.equal_range(id);
    for (auto trackIt = range.first; trackIt != range.second; ++trackIt)
    {
        MIX_Track* track = trackIt->second;
        if (track != nullptr && MIX_TrackPlaying(track)) {
            return true;
        }
    }

    return false;
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

void Managers::AudioManager::ReloadAudioFiles() {
    for (auto& [id, info] : m_assetList)
    {
        if (info == nullptr || !info->IsLoaded())
            continue;

        std::error_code errorCode;
        auto currentTime = std::filesystem::last_write_time(info->GetFilepath(), errorCode);
        if (errorCode || currentTime == info->lastWriteTime)
        {
            continue;
        }

        // Reload the audio file if it's not currently playing
        if(info->asset != nullptr && !IsTrackActive(id))
        {   
            ReloadAsset(id, m_mixer);
        }
    }
}

void Managers::AudioManager::SetVolumeByID(AudioID id, float volume) {
    float gain = ClampVolume(volume);

    auto range = m_activeTracks.equal_range(id);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second != nullptr)
            MIX_SetTrackGain(it->second, gain);
    }
}

void Managers::AudioManager::SetMasterVolume(float volume) {
    Config::ConfigSetter::SetVariable("master_volume", volume);
    MIX_SetMixerGain(m_mixer, ClampVolume(volume));
}

void Managers::AudioManager::RestartAudioDevice() {
    StopAllSounds();

    if (m_mixer != nullptr) {
        MIX_DestroyMixer(m_mixer);
        m_mixer = nullptr;
    }
    if (m_deviceID != 0) {
        SDL_CloseAudioDevice(m_deviceID);
        m_deviceID = 0;
    }

    m_deviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec);
    if (m_deviceID == 0) {
        GKC_ENGINE_FATAL("failed to reopen audio device!");
        return;
    }

    m_mixer = MIX_CreateMixerDevice(m_deviceID, &m_audioSpec);
    if (m_mixer == nullptr) {
        GKC_ENGINE_FATAL("audio mixer is NULL after restarting the device!");
    }
}

void Managers::AudioManager::SetAudioChannels(int channels) {
    if (channels < 1 || channels > 8) {
        Debug::Logger::LogErrorWithType(ErrorType::OutOfRange,
            "Audio channels must be between 1 and 8, got {}", channels);
        return;
    }
    if (m_audioSpec.channels == channels)
        return;

    m_audioSpec.channels = channels;
    RestartAudioDevice();
}

void Managers::AudioManager::SetAudioFrequency(int frequency) {
    if (frequency <= 0) {
        Debug::Logger::LogErrorWithType(ErrorType::OutOfRange,
            "Audio frequency must be greater than 0, got {}", frequency);
        return;
    }
    if (m_audioSpec.freq == frequency)
        return;

    m_audioSpec.freq = frequency;
    RestartAudioDevice();
}

void Managers::AudioManager::SetAudioFormat(SDL_AudioFormat format) {
    if (m_audioSpec.format == format)
        return;

    m_audioSpec.format = format;
    RestartAudioDevice();
}

void Managers::AudioManager::SetAudioFormat(std::string format) {
    auto sdlFormat = SDL_AUDIO_UNKNOWN;
    if (format == "U8") sdlFormat = SDL_AUDIO_U8;
    else if (format == "S8") sdlFormat = SDL_AUDIO_S8;
    else if (format == "S16") sdlFormat = SDL_AUDIO_S16;
    else if (format == "S32") sdlFormat = SDL_AUDIO_S32;
    else if (format == "F32") sdlFormat = SDL_AUDIO_F32;
    else return;

    m_audioSpec.format = sdlFormat;
    RestartAudioDevice();
}

void Managers::AudioManager::CreateAudioGroup(const string& name, float maxVolume) {
    if (name.empty()) {
        Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "Audio group name is empty!");
        return;
    }
    if (m_audioGroupList.contains(name)) {
        GKC_ENGINE_WARNING("Audio group '{}' already exists!", name);
        return;
    }

    m_audioGroupList.emplace(name, Audio::AudioGroup(maxVolume));
    GKC_ENGINE_INFO("Created audio group '{}'", name);
}

void Managers::AudioManager::ApplyGroupVolume(const Audio::AudioGroup& group) {
    float gain = ClampVolume(group.GetCurrentVolume());

    for (AudioID id : group.GetAudioList()) {
        auto range = m_activeTracks.equal_range(id);
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second != nullptr)
                MIX_SetTrackGain(it->second, gain);
        }
    }
}

void Managers::AudioManager::SetAudioVolumeByGroup(const string& name, float volume) {
    auto it = m_audioGroupList.find(name);
    if (it == m_audioGroupList.end()) {
        Debug::Logger::LogErrorWithType(ErrorType::AudioGroupNotFound, "Audio group '{}' doesn't exist!", name);
        return;
    }

    it->second.SetVolume(volume);   
    ApplyGroupVolume(it->second);
}

void Managers::AudioManager::DeleteAudioGroup(const string& name) {
    auto it = m_audioGroupList.find(name);
    if (it == m_audioGroupList.end()) {
        Debug::Logger::LogErrorWithType(ErrorType::AudioGroupNotFound, "Audio group '{}' to delete doesn't exist!", name);
        return;
    }

    m_audioGroupList.erase(it);
}

Audio::AudioGroup Managers::AudioManager::GetAudioGroupByName(const string& name) {
    auto it = m_audioGroupList.find(name);
    if (it == m_audioGroupList.end()) {
        Debug::Logger::LogErrorWithType(ErrorType::AudioGroupNotFound, "Audio group '{}' doesn't exist!", name);
        return Audio::AudioGroup();
    }
    return it->second;
}
