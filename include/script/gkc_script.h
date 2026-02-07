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

namespace Galaktic::Script {

    class GKC_Script {
        public:
            explicit GKC_Script(const path& scriptPath);
            explicit GKC_Script(const string& scriptName, const string& scriptString);

            void RunScript();
            LuaCpp::LuaContext& GetContext() { return m_context; }
        private:
            LuaCpp::LuaContext m_context;
            string m_scriptName;
    };

    enum class ScriptType {
        InlineScript,
        FileScript
    };

    struct ScriptInfo {
        ScriptID scriptID_;
        GKC_Script script_;
        ScriptType scriptType_;
    };
    
    typedef map<string, ScriptInfo> Script_List;

    extern bool CheckScriptExtension(const path& path);
}