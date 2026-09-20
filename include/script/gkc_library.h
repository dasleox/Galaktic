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

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace Galaktic::Script {
    /**
     * Registers Galaktic's API into Lua, it can be used in all Lua scripts, however
     * it needs to be initialized first calling \c Initialize() , you can bind
     * your custom functions and call them from Lua
     * 
     * These scripts can also be called from Lua to Galaktic's API, this method is
     * used to customize for instance, the behaviour of the player in specific
     * scenes. 
     * 
     * When the script folder is created structure as this:
     * 
     * \code 
     * \App                     -> App's folder
     *      \scripts            -> Main scripts folder
     *          \local          -> Local custom scripts used in scenes
     *              \scene1     -> Specific scene scripts
     *              \shared     -> Reusable scripts in all scenes
     *          \modules        -> Custom modules usable in local scripts 
     * \endcode 
     * 
     * Modules are the first ones that are started, calling a script before
     * a required module throws an exception
     * 
     * @note When binding functions it's recommended that they have the same name 
     * in the string literal and the actual function's name
     */
    class LuaGalaktic {
        public:
            /**
             * Starts a lua_State pointer (shared by scripts and modules)
             * @return true on success, false on failure
             */
            static bool Initialize();

            /**
             * Binds a function to Lua, callable at any time
             * @tparam FunctionType function type
             * @param functionName Function's name 
             * @param func function
             */
            template <typename FunctionType>
            static void BindLuaFunction(const string &functionName, FunctionType func) {
                luabridge::getGlobalNamespace(m_luaState).addFunction(functionName.c_str(), func);
            }

            /**
             * Calls a function from Lua
             * @tparam Args Arguments of the function that is being called
             * @param functionName Function's name registered in Lua
             * @param args Arguments of the function that is being called
             * @return luabridge::LuaRef 
             */
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

            /**
             * Sets a global variable inside Lua
             * @tparam T value type
             * @param varName variable's name
             * @param value value
             */
            template <typename T>
            static void SetGlobalVariable(const string &varName, T value) {
                luabridge::setGlobal(m_luaState, value, varName.c_str());
            }

            /**
             * Gets a global variable inside Lua
             * @tparam T value type
             * @param varName variable's name
             * @return T value's type
             */
            template <typename T>
            static T GetGlobalVariable(const string &varName) {
                luabridge::LuaRef ref = luabridge::getGlobal(m_luaState, varName.c_str());
                if(ref.isNil()) {
                    GKC_ENGINE_ERROR("Global variable '{}' doesn't exist!", varName);
                    return T();
                }
                return ref.cast<T>();
            }

            /**
             * Binds Galaktic's API into Lua, this function needs to be called before using
             * Galaktic's function inside Lua
             */
            static void BindGalaktic();

            /**
             * Gets a reference to a Lua function
             * @param functionName function's name
             * @return luabridge::LuaRef reference to a Lua function,
             */
            static luabridge::LuaRef GetLuaFunction(const string &functionName) {
                auto ref = luabridge::getGlobal(m_luaState, functionName.c_str());
                if(ref.isFunction()) {
                    return ref;
                } else {
                    GKC_ENGINE_ERROR("Function '{}' doesn't exist or is not a function!", functionName);
                    return NULL;
                }
            }

            /**
             * Adds a path (folder) to a Lua module, has to be started afterwards calling
             * \c SetupModules 
             * 
             * @param path module's path
             */
            static void AddLuaModulePath(const string& path);

            /**
             * Starts all modules registered in Lua to be used in any script
             * 
             * @param scriptRootFolder root script folder
             */
            static void SetupModules(const path& scriptRootFolder);
            
            /**
             * @brief Closes the Lua state and frees all memory
             */
            static void Shutdown();

            static lua_State* GetLuaState() { return m_luaState; }

            /**
             * Gets the call context of a Lua function, it returns a
             * formatted string with the line and function name that has
             * been called to be displayed in the console. \n
             * Format: "[filename.lua -> myFunction || Line: 3]"
             * @param level Level of the Lua stack
             * @return formatted string with call context info
             */
            static string GetLuaCallContext(int level);

            /**
             * Creates a default update script, has no content inside the update function
             * can be modifiable to achieve certain behaviours
             * 
             * @param path specific scene folder
             */
            static void CreateDefaultUpdateFile(const path& path);

            /**
             * Creates a default player script, it adds default movement and health
             * mechanics, this function is used when a app has been just created
             * or when the player file hasn't been found.
             * 
             * @param path shared's folder
             */
            static void CreateDefaultPlayerFile(const path& path);

            static void PrintLuaStack();
        private:
            static lua_State* m_luaState;
            static vector<string> m_modulePathList;

        private:
            // Global constants (Keys and Click types)
            static void SetKeysToLua();
            static void SetMouseClicksToLua();

            // Galaktic's API binding helpers
            static void BindAudioFunctions();
            static void BindAnimationFunctions();
            static void BindScriptFunctions();
            static void BindEntityFunctions();
            static void BindKeyboardFunctions();
            static void BindMouseFunctions();
            static void BindLoggingFunctions();

            /**
             * Automatically adds all the module folders needed to start them
             * @param sriptModulesPath script's module folder
             */
            static void AddModuleFolders(const path& sriptModulesPath);


    };
}