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
#include <core/gkc_error.h>

namespace Galaktic::Config
{
    /**
     * @class ConfigFile
     * @brief Reads and updates configuration variables while preserving other file lines.
     */
    class ConfigFile
    {
        private:
            std::filesystem::path m_path;
            std::vector<std::string> m_lines;
            std::unordered_map<std::string, size_t> m_variableToLineMap;
        private:
            /**
             * @brief Converts a typed configuration value to the file's textual representation.
             * @param value Value to convert, store or inspect
             */
            template<typename T>
            std::string ToConfigString(const T& value)
            {
                if constexpr (std::is_same_v<T, std::string>)
                {
                    return value;
                }
                else if constexpr (std::is_same_v<T, char>)
                {
                    return std::string(1, value);
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    std::ostringstream ss;
                    ss << std::setprecision(8) << value;
                    return ss.str();
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    std::ostringstream ss;
                    ss << std::setprecision(8) << value;
                    return ss.str();
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    return value ? "true" : "false";
                }
                else 
                {
                    return std::to_string(value);
                }

            }
            
            /**
             * @brief Parses a configuration value, returning std::nullopt on conversion failure.
             * @param raw Raw configuration value
             */
            template<typename T>
            static std::optional<T> FromConfigString(const std::string& raw)
            {
                if constexpr (std::is_same_v<T, std::string>)
                    return raw;

                else if constexpr (std::is_same_v<T, char>)
                {
                    if(raw.empty()) return std::nullopt;
                    return raw[0];
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    if(raw == "true"  || raw == "1") return true;
                    if(raw == "false" || raw == "0") return false;
                    return std::nullopt;
                }
                else
                {
                    std::istringstream ss(raw);
                    T value{};
                    if(!(ss >> value)) return std::nullopt;
                    return value;
                }
            }

            /**
             * @brief Adds or replaces a configuration variable in memory; Save writes it to disk.
             * @param varName Configuration or Lua variable name
             * @param value Value to convert, store or inspect
             * @return true on success, false otherwise
             */
            bool SetRaw(const std::string& varName, const std::string& value);
            /**
             * @brief Returns the trimmed raw variable value, or std::nullopt when absent.
             * @param varName Configuration or Lua variable name
             */
            std::optional<std::string> GetRaw(const std::string& varName) const;

        public:
            /**
             * @brief Stores the configuration filepath and attempts to load its contents.
             * @param filepath Filepath to load or store
             */
            explicit ConfigFile(const std::filesystem::path& filepath);
            
            /**
             * @brief Loads configuration lines and rebuilds the variable-name lookup map.
             * @return true on success, false otherwise
             */
            bool Load();
            /**
             * @brief Writes the stored configuration lines to the configured filepath.
             * @return true on success, false otherwise
             */
            bool Save() const;
            /**
             * @brief Checks whether the requested component or configuration variable exists.
             * @param varName Configuration or Lua variable name
             * @return true on success, false otherwise
             */
            bool Has(const std::string& varName) const;

            /**
             * @brief Converts and stores a configuration value in memory; Save writes it to disk.
             * @param varName Configuration or Lua variable name
             * @param value Value to convert, store or inspect
             * @return true on success, false otherwise
             */
            template<typename T>
            bool SetValue(const std::string& varName, const T& value)
            {
                return SetRaw(varName, ToConfigString(value));
            }

            /**
             * @brief Returns a converted configuration value, or std::nullopt if absent or unparseable.
             * @param varName Configuration or Lua variable name
             */
            template<typename T>
            std::optional<T> GetValue(const std::string& varName) const
            {
                auto raw = GetRaw(varName);
                if(!raw) return std::nullopt;
                return FromConfigString<T>(*raw);
            }

            bool IsValid() const
            {
                return !m_path.empty() && !m_lines.empty();
            }

            const std::filesystem::path& GetPath() const
            {
                return m_path;
            }
    };
}