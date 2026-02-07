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
#include <script/gkc_script.h>

namespace Galaktic::Core::Managers {
    class ScriptManager {
        public:
            ScriptManager(const path& folder);

            static void AddInlineScript(const string& scriptName, const string& script);
            static void AddScriptFromFile(const path& scriptPath);

            static void DeleteScriptFromList(const string& scriptName);

            static Script::GKC_Script* GetScriptFromName(const string& scriptName);
            static Script::GKC_Script* GetScriptFromID(ScriptID id);

            static Script::Script_List& GetScriptList() { return m_scriptList; }
        private:
            static Script::Script_List m_scriptList;
            static std::multimap<ScriptID, string> m_IDToNameList;
    };
}