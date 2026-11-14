#pragma once
#include <config/gkc_config_file.h>
#include <core/managers/gkc_config_manager.h>

namespace Galaktic::Config
{
    // Configuration supplies defaults once, before scene/runtime API overrides.
    template<typename T, typename Validator>
    T GetValueOrDefault(const std::string& filename, const std::string& name,
        const T& fallback, Validator valid)
    {
        const auto* file = Core::Managers::ConfigManager::GetConfigFile(filename);
        if(!file) return fallback;
        if(!file->IsValid()) return fallback;
        auto value = file->GetValue<T>(name);
        return value && valid(*value) ? *value : fallback; // If value exists and is valid, return it; otherwise, return fallback.
    }

    template<typename T>
    T GetValueOrDefault(const std::string& filename, const std::string& name, const T& fallback)
    {
        return GetValueOrDefault(filename, name, fallback, [](const T&) { return true; });
    }

    // Relative configuration paths are relative to the project, never the process cwd.
    std::filesystem::path GetDirectoryOrDefault(const std::string& name,
        const std::filesystem::path& project, const std::filesystem::path& fallback);

    struct AudioSettings
    {
        SDL_AudioSpec spec{SDL_AUDIO_F32, 2, 48000};
        float masterVolume = 1.f;
        float maxVolume = 1.f;
    };

    AudioSettings ReadAudioSettings();
}
