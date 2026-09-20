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

namespace Galaktic::Core {
    class Scene;

    struct DebugInformation {
        size_t m_ramUsage;
        size_t m_ramAvailable;
        string m_engineName;
        string m_displayInfo;
        float m_fps;
        float m_playerXCoordinate = 0.f;
        float m_playerYCoordinate = 0.f;

        DebugInformation();
    };

    namespace Managers 
    {
        class SceneManager;
    }

    enum class CommandTypes : uint32_t {
        // Scene Related
        Unknown = 0,
        LoadScene,
        SaveCurrentScene,
        ShowSceneList,
        PauseToggleScene,
        
        // Entity Related
        CreateStaticObject,
        CreatePhysicsObject,
        DeleteEntity,
        TeleportEntity,
        SetRotationEntity,
        DisplayEntityInformation,
        PrintEntityList,

        // Scriting/Lua Related
        ExecuteScript,
        PrintLuaStack,

        // Actions Related
        BindAction,

        // App Related
        FullscreenToggle,
        CheatsToggle,    
        ClearConsole,
        HelpConsole,

        // Sounds
        PlaySound,
        StopSound,
        StopAllSounds,
        PrintSoundList, 
        
        // Debug
        ShowFPSToggle,
        ShowWireframeToggle,
        ShowCollidersToggle,
        ShowDebugInformationToggle,
    };

    struct CommandWrapper   
    {
        CommandWrapper() {}
        CommandWrapper(const string& command, const string& args, CommandTypes type)
        {
            this->command = command;
            this->args = args;
            this->type = type;
        }

        constexpr bool IsEmpty() {
            return command.empty() || type == CommandTypes::Unknown; 
        }
        constexpr bool HasArguments()
        {
            return !args.empty();
        }
        string command = "";
        string args = "";
        CommandTypes type = CommandTypes::Unknown;
    };

    class Console {
        private:
            Scene* m_currentScene = nullptr;
            Managers::SceneManager* m_sceneManager = nullptr;
            unique_ptr<DebugInformation> m_pDebugInfo = nullptr;
            shared_ptr<Debug::ConsoleMutex_mt> m_sink = nullptr;

            bool m_cheatsToggle = false;
            bool m_isVisible = true;

        public:
            Console(shared_ptr<Debug::ConsoleMutex_mt> sink);

            void Display(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight);
            void GetSceneContext(Scene* scene);
            void ExecuteCommand(const string& command);
        private:
            void CommandSwitch(const CommandWrapper& wrapper);

            CommandWrapper NoArgumentCommands(const string& command);
            CommandWrapper ArgumentCommands(const string& command, const string& args);
            void SceneCommands(const CommandWrapper& wrapper);
            void EntityCommands(const CommandWrapper& wrapper);
            void ScriptCommands(const CommandWrapper& wrapper);
            void AppCommands(const CommandWrapper& wrapper);
            void SoundCommands(const CommandWrapper& wrapper);
            void DebugCommands(const CommandWrapper& wrapper);

            void ClearConsole();
            void DisplayHelp();
            void ShowFPS();
            void ShowDebugInformation();
            
            void InvalidArgumentError(const string& message);

            vector<string> GetListOfArguments(const string& args);
            SDL_Color LevelToColor(spdlog::level::level_enum level);      
    };
}