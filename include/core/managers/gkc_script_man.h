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
#include <core/managers/gkc_manager.h>
#include <script/gkc_script.h>

namespace Galaktic::Core::Managers {
    /**
     * @class ScriptManager
     * @brief Stores Lua scripts and resolves them by name or ID.
     */
    class ScriptManager {
        public:
            /**
             * @brief Initializes this manager using the supplied project context.
             * @param folder Folder containing the managed files
             * @param luaState Borrowed Lua state used to load and execute the script
             */
            ScriptManager(const std::string& folder, lua_State* luaState);

            /**
             * @brief Registers a Lua script supplied as a string.
             * @param scriptName Name identifying the Lua script
             * @param script Lua source code
             */
            static void AddInlineScript(const std::string& scriptName, const std::string& script);
            /**
             * @brief Registers a Lua script loaded from a file.
             * @param scriptPath Lua script filepath
             */
            static void AddScriptFromFile(const std::string& scriptPath);

            /**
             * @brief Invokes the script's named global entry-point function.
             * @param scriptName Name identifying the Lua script
             */
            static void RunScript(const std::string& scriptName);
            /**
             * @brief Removes a registered script by name.
             * @param scriptName Name identifying the Lua script
             */
            static void DeleteScriptFromList(const std::string& scriptName);

            /**
             * @brief Returns shared ownership of the named script, or nullptr when absent.
             * @param scriptName Name identifying the Lua script
             */
            static std::shared_ptr<Script::GKC_Script> GetScriptFromName(const std::string& scriptName);
            /**
             * @brief Returns shared ownership of the script ID, or nullptr when absent.
             * @param id Entity or asset ID used by this operation
             */
            static std::shared_ptr<Script::GKC_Script> GetScriptFromID(ScriptID id);

            /**
             * @brief Returns mutable access to the registered scripts.
             */
            static Script::Script_List& GetScriptList() { return m_scriptList; }
        private:
            static Script::Script_List m_scriptList;
            static std::unordered_map<ScriptID, std::string> m_IDToNameList;
            static lua_State* m_luaState;
            /**
             * @brief Executes the engine's Lua module in the current scripting state.
             */
            static void ExecuteGalakticModule();
    };
}