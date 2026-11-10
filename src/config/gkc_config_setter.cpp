#include <config/gkc_config_setter.h>
#include <core/managers/gkc_audio_man.h>
#include <core/managers/gkc_config_manager.h>
#include <config/gkc_config_file.h>
#include <core/gkc_logger.h>
#include <core/gkc_error.h>
#include <filesys/gkc_filesys.h>

using namespace Galaktic;
using namespace Galaktic::Config;
using namespace Galaktic::Core::Managers;

#define CHECK_OPTIONAL_VALUE(opt, func, varName, file) \
    do { \
        if((opt).has_value()) { \
            func(*opt); \
        } else { \
            Debug::Logger::LogErrorWithType(ErrorType::InvalidValue, "Failed to set '{}' value from '{}' file!", varName, file->GetPath().string()); \
        } \
    } while(0)

namespace
{
    //@todo Fix the bug with configurations variables in filesw
    template<typename T>
    void SetConfigVariable(const std::string& variableName, const T& value)
    {
        if(variableName.empty())
        {
            Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "Configuration variable name is empty!");
            return;
        }

        for(auto& filename : ConfigManager::GetConfigFileNames())
        {
            GKC_ENGINE_INFO("Searching for '{}' in '{}'", variableName, filename);
            auto configFile = ConfigManager::GetConfigFile(filename);
            if(!configFile) return;

            if(!configFile->IsValid() || !configFile->Has(variableName))
                continue;

            if(!configFile->SetValue(variableName, value))
            {
                Debug::Logger::LogErrorWithType(ErrorType::InvalidValue, "Failed to set '{}' value in '{}' file!", variableName, configFile->GetPath().string());
                return;
            }

            if(!configFile->Save())
                Debug::Logger::LogErrorWithType(ErrorType::SaveFailed, "Failed to save '{}' value to '{}' file!", variableName, configFile->GetPath().string());
            return;
        }

        Debug::Logger::LogErrorWithType(ErrorType::InvalidArgument, "Configuration variable '{}' was not found in the standard configuration files!", variableName);
    }
}

void ConfigSetter::SetVariable(const std::string& variableName, const double& value)
{
    SetConfigVariable(variableName, value);
}

void ConfigSetter::SetVariable(const std::string& variableName, const float& value)
{   
    SetConfigVariable(variableName, value);
}

void ConfigSetter::SetVariable(const std::string& variableName, const int& value)
{
    SetConfigVariable(variableName, value);
}

void ConfigSetter::SetVariable(const std::string& variableName, const std::string& value)
{
    SetConfigVariable(variableName, value);
}

void ConfigSetter::SetVariable(const std::string& variableName, const bool& value)
{
    SetConfigVariable(variableName, value);
}

void ConfigSetter::SetVariable(const std::string& variableName, const char& value)
{
    SetConfigVariable(variableName, value);
}

void ConfigSetter::SetAudioConfigurationFromFile()
{
    auto configFile = ConfigManager::GetConfigFile("audio.cfg");
    if(!configFile) return;

    if(!configFile->IsValid())
    {
        Debug::Logger::LogErrorWithType(ErrorType::NoConfigFileFound, "Audio configuration file 'audio.cfg' not found!");
        return;
    }

    auto audioChannelsOpt = configFile->GetValue<int>("audio_channels");
    CHECK_OPTIONAL_VALUE(audioChannelsOpt, AudioManager::SetAudioChannels, "audio_channels", configFile);

    auto audioFrequencyOpt = configFile->GetValue<int>("audio_frequency");
    CHECK_OPTIONAL_VALUE(audioFrequencyOpt, AudioManager::SetAudioFrequency, "audio_frequency", configFile);

    auto audioFormatOpt = configFile->GetValue<std::string>("audio_format");
    CHECK_OPTIONAL_VALUE(audioFormatOpt, AudioManager::SetAudioFormat, "audio_format", configFile);

    auto audioMasterVolumeOpt = configFile->GetValue<float>("master_volume");
    CHECK_OPTIONAL_VALUE(audioMasterVolumeOpt, AudioManager::SetMasterVolume, "master_volume", configFile);

}


#undef CHECK_OPTIONAL_VALUE