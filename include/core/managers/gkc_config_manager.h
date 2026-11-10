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

namespace Galaktic::Config 
{
    class ConfigFile;
}

namespace Galaktic::Core::Managers
{
    /**
     * @class ConfigManager
     * @brief Creates and retrieves the project's named configuration files.
     */
    class ConfigManager
    {
        private:
            static std::filesystem::path m_folderPath;
            static std::unordered_map<std::string, Config::ConfigFile> m_configFileMap;
            /**
             * @brief Creates the default audio configuration file.
             * @return true on success, false otherwise
             */
            static bool CreateAudioConfigFile();
            /**
             * @brief Creates the default input-binding configuration file.
             * @return true on success, false otherwise
             */
            static bool CreateBindConfigFile();
            /**
             * @brief Creates the default general configuration file.
             * @return true on success, false otherwise
             */
            static bool CreateGeneralConfigFile();
        public:
            /**
             * @brief Sets the configuration folder and initializes its standard files.
             * @param folderPath Folder containing the managed files
             * @return true on success, false otherwise
             */
            static bool SetFolder(const std::filesystem::path& folderPath);
            /**
             * @brief Loads and registers a configuration file.
             * @param filepath Filepath to load or store
             * @return true on success, false otherwise
             */
            static bool AddConfigFile(const std::filesystem::path& filepath);
            /**
             * @brief Returns a copy of a registered configuration file; changes to the copy do not update the manager.
             * @param filename Filename, including its extension when present
             */
            static Config::ConfigFile* GetConfigFile(const std::string& filename);

            static std::vector<std::string> GetConfigFileNames();
    };
}
