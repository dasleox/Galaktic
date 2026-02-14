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
            explicit GKC_Script(const path& scriptPath, lua_State* luaState);
            explicit GKC_Script(const string& scriptName, const string& scriptString, lua_State* luaState);

            void RunScript();
            lua_State* GetLuaState() { return m_luaState; }
            
            template<typename T>
            T GetGlobal(const string& name) {
                return luabridge::getGlobal(m_luaState, name.c_str());
            }

            template<typename T>
            void SetGlobal(const string& name, const T& value) {
                luabridge::setGlobal(m_luaState, value, name.c_str());
            }
            
        private:
            lua_State* m_luaState;
            string m_scriptName;
    };

    enum class ScriptType {
        InlineScript,
        FileScript
    };

    struct ScriptInfo {
        ScriptID scriptID_;
        shared_ptr<Script::GKC_Script> script_;
        ScriptType scriptType_;
    };
    
    typedef unordered_map<string, ScriptInfo> Script_List;

    extern bool CheckScriptExtension(const path& path);
}