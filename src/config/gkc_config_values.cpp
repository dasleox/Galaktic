#include <config/gkc_config_values.h>

using namespace Galaktic::Config;

std::filesystem::path Galaktic::Config::GetDirectoryOrDefault(const std::string& name,
    const std::filesystem::path& project, const std::filesystem::path& fallback)
{
    auto value = GetValueOrDefault<std::string>("general.cfg", name, "");
    if(!value.empty() && value.find('\0') == std::string::npos)
    {
        auto candidate = (project / value).lexically_normal();
        std::error_code error;
        if(std::filesystem::is_directory(candidate, error) && !error)
            return candidate;
    }
    return (project / fallback).lexically_normal();
}

AudioSettings Galaktic::Config::ReadAudioSettings()
{
    AudioSettings settings;
    settings.spec.channels = GetValueOrDefault<int>("audio.cfg", "audio_channels",
        settings.spec.channels, [](int value) { return value >= 1 && value <= 8; });
    settings.spec.freq = GetValueOrDefault<int>("audio.cfg", "audio_frequency",
        settings.spec.freq, [](int value) { return value > 0; });
    settings.masterVolume = GetValueOrDefault<float>("audio.cfg", "master_volume",
        settings.masterVolume, [](float value) { return value >= 0.f && value <= 1.f; });
    settings.maxVolume = GetValueOrDefault<float>("audio.cfg", "max_volume",
        settings.maxVolume, [](float value) { return value >= 0.f && value <= 1.f; });
    const auto format = GetValueOrDefault<std::string>("audio.cfg", "audio_format", "F32");
    if(format == "U8") settings.spec.format = SDL_AUDIO_U8;
    else if(format == "S8") settings.spec.format = SDL_AUDIO_S8;
    else if(format == "S16") settings.spec.format = SDL_AUDIO_S16;
    else if(format == "S32") settings.spec.format = SDL_AUDIO_S32;
    else if(format == "F32") settings.spec.format = SDL_AUDIO_F32;
    return settings;
}
