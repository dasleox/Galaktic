#include <audio/gkc_audio.h>
#include <core/managers/gkc_audio_man.h>
#include "core/gkc_logger.h"
#include <config/gkc_config_setter.h>
#include <config/gkc_config_values.h>
#include "filesys/gkc_filesys.h"
#include <audio/gkc_audio_group.h>
#include <cmath>

using std::string;
using std::vector;

using namespace Galaktic;
using namespace Galaktic::Core;

static const vector<string> AUDIO_EXTENSIONS = {".wav", ".ogg", ".mp3", ".flac", ".mod", ".mid", ".mp3"};

namespace {
    void CollectFinishedTracks(std::multimap<AudioID, MIX_Track*>& tracks) {
        for(auto it = tracks.begin(); it != tracks.end();) {
            auto* track = it->second;
            if(track != nullptr && (MIX_TrackPlaying(track) || MIX_TrackPaused(track))) {
                ++it;
                continue;
            }
            if(track != nullptr) MIX_DestroyTrack(track);
            it = tracks.erase(it);
        }
    }

    bool StopTrackWithFade(MIX_Track* track, Sint64 fadeOutMs) {
        if(track == nullptr) return false;
        const auto frames = fadeOutMs > 0 ? MIX_TrackMSToFrames(track, fadeOutMs) : 0;
        if(frames < 0 || !MIX_StopTrack(track, frames)) {
            GKC_ENGINE_ERROR("Failed to stop audio playback: {}", SDL_GetError());
            return false;
        }
        return true;
    }
}

std::multimap<AudioID, MIX_Track*> Managers::AudioManager::m_activeTracks;
SDL_AudioSpec Managers::AudioManager::m_audioSpec{};
SDL_AudioDeviceID Managers::AudioManager::m_deviceID;
MIX_Mixer* Managers::AudioManager::m_mixer = nullptr;
std::unordered_map<string, Audio::AudioGroup> Managers::AudioManager::m_audioGroupList;
float Managers::AudioManager::m_maxVolume = 1.f;
float Managers::AudioManager::m_masterVolume = 1.f;
bool Managers::AudioManager::m_isPaused = false;

Managers::AudioManager::AudioManager(const string &folder) {
    Config::ConfigSetter::SetAudioConfigurationFromFile();

    if(!OpenAudioDevice()) {
        const auto defaults = Config::AudioSettings{}.spec;
        if(m_audioSpec.channels == defaults.channels && m_audioSpec.freq == defaults.freq &&
            m_audioSpec.format == defaults.format) {
            Debug::Logger::LogFatalWithType(ErrorType::DeviceNotFound, "Failed to open the default audio device!");
            return;
        }
        GKC_ENGINE_WARNING("Configured audio specification is unavailable; using Galaktic defaults.");
        m_audioSpec = defaults;
        if(!OpenAudioDevice()) {
            Debug::Logger::LogFatalWithType(ErrorType::DeviceNotFound, "Failed to open the default audio device!");
            return;
        }
    }


    auto files = Filesystem::GetFilenamesInFolder(folder);
    for (auto& file : files) {
        if (Filesystem::CheckExtension(file, AUDIO_EXTENSIONS))
            LoadAndRegisterAsset(file, m_mixer);
    }
}

void Managers::AudioManager::LoadConfiguration() {
    const auto settings = Config::ReadAudioSettings();
    const bool changed = m_audioSpec.channels != settings.spec.channels ||
        m_audioSpec.freq != settings.spec.freq || m_audioSpec.format != settings.spec.format;
    m_audioSpec = settings.spec;
    m_maxVolume = settings.maxVolume;
    m_masterVolume = std::min(settings.masterVolume, m_maxVolume);
    if(m_mixer != nullptr) {
        if(changed) RestartAudioDevice();
        else MIX_SetMixerGain(m_mixer, m_masterVolume);
    }
}

bool Managers::AudioManager::OpenAudioDevice() {
    m_deviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &m_audioSpec);
    if(m_deviceID == 0) return false;
    SDL_PauseAudioDevice(m_deviceID);
    m_mixer = MIX_CreateMixerDevice(m_deviceID, &m_audioSpec);
    if(m_mixer == nullptr) {
        SDL_CloseAudioDevice(m_deviceID);
        m_deviceID = 0;
        return false;
    }
    MIX_SetMixerGain(m_mixer, m_masterVolume);
    return true;
}

void Managers::AudioManager::PlayAudioFile(const string& name, int loops) {
    CollectFinishedTracks(m_activeTracks);
    if(m_activeTracks.size() >= MAX_TRACKS_QUANTITY)
        return;
    if(m_mixer == nullptr) return;

    auto* audioInfo = GetAssetInfo(name);
    if (audioInfo != nullptr && audioInfo->IsLoaded() && audioInfo->GetAsset() != nullptr) {
        auto audioFile = audioInfo->GetAsset()->GetAudioSample();
        auto id = GetAssetIDByName(name);

        if(id == 0)
        {
            Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid audio ID!: {}", name);
            return;
        }
        MIX_Track* track = MIX_CreateTrack(m_mixer);
        const auto options = SDL_CreateProperties();
        const bool started = track != nullptr && options != 0 &&
            SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, std::max(loops, -1)) &&
            MIX_SetTrackAudio(track, audioFile) && MIX_PlayTrack(track, options);
        if(options != 0) SDL_DestroyProperties(options);
        if(!started || (m_isPaused && !MIX_PauseTrack(track))) {
            GKC_ENGINE_ERROR("Failed to play '{}': {}", name, SDL_GetError());
            if(track != nullptr) MIX_DestroyTrack(track);
            return;
        }

        m_activeTracks.emplace(id, track);
    }
}

void Managers::AudioManager::PlayMusicFile(const string &name) {
    PlayAudioFile(name, -1);
}

void Managers::AudioManager::StopSound(const string &name, Sint64 fadeOutMs) {
    CollectFinishedTracks(m_activeTracks);
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
            StopTrackWithFade(track, fadeOutMs);
            CollectFinishedTracks(m_activeTracks);
        }
    }
}

bool Managers::AudioManager::IsTrackActive(AudioID id) {
    const auto range = m_activeTracks.equal_range(id);
    for (auto trackIt = range.first; trackIt != range.second; ++trackIt)
    {
        MIX_Track* track = trackIt->second;
        if (track != nullptr && (MIX_TrackPlaying(track) || MIX_TrackPaused(track))) {
            return true;
        }
    }

    return false;
}

void Managers::AudioManager::StopAllTracksFromSound(const string &name, Sint64 fadeOutMs) {
    CollectFinishedTracks(m_activeTracks);
    auto* audioInfo = GetAssetInfo(name);
    if (audioInfo != nullptr) {
        auto id = GetAssetIDByName(name);

        if(id == 0)
        {
            Debug::Logger::LogErrorWithType(ErrorType::AssetInvalid, "Invalid audio ID!: {}", name);
            return;
        }

        auto range = m_activeTracks.equal_range(id);
        for (auto iter = range.first; iter != range.second; ++iter) {
            StopTrackWithFade(iter->second, fadeOutMs);
        }
        CollectFinishedTracks(m_activeTracks);
    }
}

void Managers::AudioManager::StopAllSounds(Sint64 fadeOutMs) {
    if(m_mixer != nullptr && !MIX_StopAllTracks(m_mixer, std::max<Sint64>(fadeOutMs, 0)))
        GKC_ENGINE_ERROR("Failed to stop all audio playback: {}", SDL_GetError());
    CollectFinishedTracks(m_activeTracks);
}

void Managers::AudioManager::RegisterTrack(AudioID id, MIX_Track *track) {
    m_activeTracks.emplace(id, track);
}

void Managers::AudioManager::SetPaused(bool paused) {
    if(m_isPaused == paused) return;
    m_isPaused = paused;
    if(m_mixer == nullptr) return;

    const bool success = paused ? MIX_PauseAllTracks(m_mixer) : MIX_ResumeAllTracks(m_mixer);
    if(!success) GKC_ENGINE_ERROR("Failed to {} audio playback: {}", paused ? "pause" : "resume", SDL_GetError());
}

void Managers::AudioManager::ReloadAudioFiles() {
    CollectFinishedTracks(m_activeTracks);
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
    if(!std::isfinite(volume)) return;
    m_masterVolume = std::min(ClampVolume(volume), m_maxVolume);
    Config::ConfigSetter::SetVariable("master_volume", m_masterVolume, "audio.cfg");
    if(m_mixer != nullptr) MIX_SetMixerGain(m_mixer, m_masterVolume);
}

void Managers::AudioManager::SetMaxVolume(float volume) {
    if(!std::isfinite(volume)) return;
    m_maxVolume = ClampVolume(volume);
    Config::ConfigSetter::SetVariable("max_volume", m_maxVolume, "audio.cfg");
    SetMasterVolume(m_masterVolume);
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

    if(!OpenAudioDevice()) {
        GKC_ENGINE_FATAL("failed to reopen audio device!");
    }
}

void Managers::AudioManager::SetAudioChannels(int channels) {
    if (channels < 1 || channels > 8) {
        Debug::Logger::LogErrorWithType(ErrorType::OutOfRange,
            "Audio channels must be between 1 and 8, got {}", channels);
        return;
    }
    Config::ConfigSetter::SetVariable("audio_channels", channels, "audio.cfg");
    if (m_audioSpec.channels == channels)
        return;

    m_audioSpec.channels = channels;
    if(m_mixer != nullptr) RestartAudioDevice();
}

void Managers::AudioManager::SetAudioFrequency(int frequency) {
    if (frequency <= 0) {
        Debug::Logger::LogErrorWithType(ErrorType::OutOfRange,
            "Audio frequency must be greater than 0, got {}", frequency);
        return;
    }
    Config::ConfigSetter::SetVariable("audio_frequency", frequency, "audio.cfg");
    if (m_audioSpec.freq == frequency)
        return;

    m_audioSpec.freq = frequency;
    if(m_mixer != nullptr) RestartAudioDevice();
}

void Managers::AudioManager::SetAudioFormat(SDL_AudioFormat format) {
    string configuredFormat;
    switch(format) {
        case SDL_AUDIO_U8: configuredFormat = "U8"; break;
        case SDL_AUDIO_S8: configuredFormat = "S8"; break;
        case SDL_AUDIO_S16: configuredFormat = "S16"; break;
        case SDL_AUDIO_S32: configuredFormat = "S32"; break;
        case SDL_AUDIO_F32: configuredFormat = "F32"; break;
        default: return;
    }
    Config::ConfigSetter::SetVariable("audio_format", configuredFormat, "audio.cfg");
    if (m_audioSpec.format == format)
        return;

    m_audioSpec.format = format;
    if(m_mixer != nullptr) RestartAudioDevice();
}

void Managers::AudioManager::SetAudioFormat(std::string format) {
    auto sdlFormat = SDL_AUDIO_UNKNOWN;
    if (format == "U8") sdlFormat = SDL_AUDIO_U8;
    else if (format == "S8") sdlFormat = SDL_AUDIO_S8;
    else if (format == "S16") sdlFormat = SDL_AUDIO_S16;
    else if (format == "S32") sdlFormat = SDL_AUDIO_S32;
    else if (format == "F32") sdlFormat = SDL_AUDIO_F32;
    else return;

    SetAudioFormat(sdlFormat);
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
