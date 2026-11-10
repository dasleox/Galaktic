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

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace Galaktic::Script {
    /**
     * @class GKC_Script
     * @brief Borrows a Lua state to load a script and invoke its named entry point.
     */
    class GKC_Script {
        public:
            /**
             * @brief Loads and executes Lua source using the borrowed Lua state.
             * @param scriptPath Lua script filepath
             * @param luaState Borrowed Lua state used to load and execute the script
             */
            explicit GKC_Script(const std::filesystem::path& scriptPath, lua_State* luaState);
            /**
             * @brief Loads and executes Lua source using the borrowed Lua state.
             * @param scriptName Name identifying the Lua script
             * @param scriptString Lua source code
             * @param luaState Borrowed Lua state used to load and execute the script
             */
            explicit GKC_Script(const std::string& scriptName, const std::string& scriptString, lua_State* luaState);

            /**
             * @brief Invokes the script's named global entry-point function.
             */
            void RunScript();
            /**
             * @brief Returns the borrowed Lua state used by this script or scripting interface.
             */
            lua_State* GetLuaState() { return m_luaState; }
            
            /**
             * @brief Reads a typed global value from the borrowed Lua state.
             * @param name Name used to identify the requested entry
             */
            template<typename T>
            T GetGlobal(const std::string& name) {
                return luabridge::getGlobal(m_luaState, name.c_str());
            }

            /**
             * @brief Writes a typed global value into the borrowed Lua state.
             * @param name Name used to identify the requested entry
             * @param value Value to convert, store or inspect
             */
            template<typename T>
            void SetGlobal(const std::string& name, const T& value) {
                luabridge::setGlobal(m_luaState, value, name.c_str());
            }
            
        private:
            lua_State* m_luaState;
            std::string m_scriptName;
    };

    /**
     * @enum ScriptType
     * @brief Identifies whether a script comes from a file or an inline string.
     */
    enum class ScriptType {
        InlineScript,
        FileScript
    };

    /**
     * @struct ScriptInfo
     * @brief Stores a script's ID, type and shared ownership.
     */
    struct ScriptInfo {
        ScriptID scriptID_;
        std::shared_ptr<Script::GKC_Script> script_;
        ScriptType scriptType_;
    };
    
    typedef std::unordered_map<std::string, ScriptInfo> Script_List;

    /**
     * @brief Checks whether the filepath exists and has the supported .lua extension.
     * @param path Source asset filepath
     * @return true if the filepath exists and has the supported .lua extension, false otherwise
     */
    extern bool CheckScriptExtension(const std::filesystem::path& path);
}