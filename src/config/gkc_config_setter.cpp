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

namespace
{
    template<typename T>
    void SetConfigVariable(const std::string& variableName, const T& value, const std::string& filename)
    {
        if(variableName.empty())
        {
            Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "Configuration variable name is empty!");
            return;
        }

        const auto filenames = filename.empty() ? ConfigManager::GetConfigFileNames()
            : std::vector<std::string>{filename};
        for(const auto& configFilename : filenames)
        {
            auto configFile = ConfigManager::GetConfigFile(configFilename);
            if(!configFile) continue;

            if(!configFile->IsValid() || (filename.empty() && !configFile->Has(variableName)))
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

void ConfigSetter::SetVariable(const std::string& variableName, const double& value, const std::string& filename)
{
    SetConfigVariable(variableName, value, filename);
}

void ConfigSetter::SetVariable(const std::string& variableName, const float& value, const std::string& filename)
{   
    SetConfigVariable(variableName, value, filename);
}

void ConfigSetter::SetVariable(const std::string& variableName, const int& value, const std::string& filename)
{
    SetConfigVariable(variableName, value, filename);
}

void ConfigSetter::SetVariable(const std::string& variableName, const std::string& value, const std::string& filename)
{
    SetConfigVariable(variableName, value, filename);
}

void ConfigSetter::SetVariable(const std::string& variableName, const bool& value, const std::string& filename)
{
    SetConfigVariable(variableName, value, filename);
}

void ConfigSetter::SetVariable(const std::string& variableName, const char& value, const std::string& filename)
{
    SetConfigVariable(variableName, value, filename);
}

void ConfigSetter::SetAudioConfigurationFromFile()
{
    AudioManager::LoadConfiguration();
}
