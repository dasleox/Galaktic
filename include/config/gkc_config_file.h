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
    class ConfigFile
    {
        private:
            std::filesystem::path m_path;
            std::vector<std::string> m_lines;
            std::unordered_map<std::string, size_t> m_variableToLineMap;
        private:
            template<typename T>
            std::string ToConfigString(const T& value)
            {
                if constexpr (std::is_same_v<T, std::string>) 
                {
                    return value;
                }
                else if constexpr (std::is_same_v<T, char>) 
                {
                    return string(1, value);
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
            
            template<typename T>
            static std::optional<T> FromConfigString(const string& raw)
            {
                if constexpr (std::is_same_v<T, string>)      
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

            bool SetRaw(const string& varName, const string& value);
            std::optional<string> GetRaw(const string& varName) const;

        public:
            explicit ConfigFile(const path& filepath);
            
            bool Load();
            bool Save() const;
            bool Has(const string& varName) const;

            template<typename T>
            bool SetValue(const string& varName, const T& value)
            {
                return SetRaw(varName, ToConfigString(value));
            }

            template<typename T>
            std::optional<T> GetValue(const string& varName) const
            {
                auto raw = GetRaw(varName);
                if(!raw) return std::nullopt;
                return FromConfigString<T>(*raw);
            }
    };
}