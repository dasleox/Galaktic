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
    /**
     * @struct BindEntry
     * @brief Stores the key, action and callback parsed from one binding directive.
     */
    struct BindEntry
    {
        string bindKey;
        string actionName;
        string callbackType;
        string callbackName;
    };  

    /**
     * @class ConfigParser
     * @brief Parses input-binding directives from configuration files.
     */
    class ConfigParser
    {
        private:
            static path m_configFilepath;
            static vector<BindEntry> m_bindEntryList;
        private:
            /**
             * @brief Parses a binding directive into its key, action and callback fields.
             * @param line Configuration line to parse
             */
            static BindEntry ParseLine(const string& line);
            /**
             * @brief Removes leading and trailing whitespace from a string.
             * @param str String to convert or inspect
             */
            static std::string Trim(const string& str);
        public:
            /**
             * @brief Reads binding directives from a configuration file.
             * @param filepath Filepath to load or store
             * @return true on success, false otherwise
             */
            static bool ParseFile(const path& filepath);
            /**
             * @brief Returns mutable access to the parsed binding entries.
             */
            static vector<BindEntry>& GetBindEntryList() { return m_bindEntryList; }
    };
}