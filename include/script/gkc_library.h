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
#include <core/gkc_logger.h>

namespace Galaktic::Script {
    class LuaGalaktic {
        public:
            static bool Initialize();

            template <typename FunctionType>
            static void BindLuaFunction(const string &functionName, FunctionType func) {
                luabridge::getGlobalNamespace(m_luaState).addFunction(functionName.c_str(), func);
            }

            template <typename... Args>
            static luabridge::LuaRef CallLuaFunction(const string &functionName, Args... args) {
                auto& func = luabridge::getGlobal(m_luaState, functionName.c_str());
                if(func.isFunction()) {
                    return func(args...);
                } else {
                    GKC_ENGINE_ERROR("Function '{}' doesn't exist or is not a function!", functionName);
                    return NULL;
                }
            }

            static bool CallLuaFunction(const string &functionName);

            template <typename T>
            static void SetGlobalVariable(const string &varName, T value) {
                luabridge::setGlobal(m_luaState, value, varName.c_str());
            }

            template <typename T>
            static T GetGlobalVariable(const string &varName) {
                luabridge::LuaRef ref = luabridge::getGlobal(m_luaState, varName.c_str());
                if(ref.isNil()) {
                    GKC_ENGINE_ERROR("Global variable '{}' doesn't exist!", varName);
                    return T();
                }
                return ref.cast<T>();
            }

            static void BindGalaktic();
            static void BindAudioFunctions();
            static void BindTextureFunctions();
            static void BindAnimationFunctions();
            static void BindScriptFunctions();
            static void BindEntityFunctions();
            static void BindKeyboardFunctions();
            static void BindMouseFunctions();

            static luabridge::LuaRef GetLuaFunction(const string &functionName) {
                auto ref = luabridge::getGlobal(m_luaState, functionName.c_str());
                if(ref.isFunction()) {
                    return ref;
                } else {
                    GKC_ENGINE_ERROR("Function '{}' doesn't exist or is not a function!", functionName);
                    return NULL;
                }
            }

            static void AddLuaModulePath(const path &path);
            static void SetupModules(const path& scriptRootFolder);
            
            /**
             * @brief Closes the Lua state and frees all memory
             */
            static void Shutdown();

            static lua_State* GetLuaState() { return m_luaState; }
            
        private:
            static lua_State* m_luaState;
            static void SetKeysToLua();
            static void SetMouseClicksToLua();
            static void AddModuleFolders(const path& sriptModulesPath);
            static vector<string> m_modulePathList;
    };
}