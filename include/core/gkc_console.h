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

namespace Galaktic::Core::Events {
    class GKC_Event;
}

namespace Galaktic::Core {
    class Scene;

    /**
     * @struct DebugInformation
     * @brief Snapshot of engine/runtime stats shown by `show_debug_info`.
     * @note RAM usage fields are currently unused: Galaktic::Debug's
     *       RAM-usage helpers are only implemented for Windows (see
     *       gkc_main.h); the Linux implementation is commented out.
     */
    struct DebugInformation {
        size_t m_ramUsage = 0;
        size_t m_ramAvailable = 0;
        string m_engineName;
        string m_displayInfo;
        float m_fps = 0.f;
        float m_playerXCoordinate = 0.f;
        float m_playerYCoordinate = 0.f;

        DebugInformation() = default;
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

    /**
     * @class Console
     * @brief In-game developer console: displays engine log output and lets
     *        the player type commands that reach the scene/ECS/scripting/
     *        audio/config systems.
     */
    class Console {
        private:
            Scene* m_currentScene = nullptr;
            Managers::SceneManager* m_sceneManager = nullptr;
            unique_ptr<DebugInformation> m_pDebugInfo = nullptr;
            shared_ptr<Debug::ConsoleMutex_mt> m_sink = nullptr;

            bool m_cheatsToggle = false;
            bool m_isVisible = true;

            /// Text currently being typed into the console's input line.
            string m_inputBuffer;

            std::stack<string> m_commandHistory;
        public:
            Console(shared_ptr<Debug::ConsoleMutex_mt> sink);

            /**
             * @brief Draws the console panel, its recent log lines, and the
             *        input line. Does nothing while `IsVisible()` is false.
             * @param renderer SDL_Renderer to draw with
             * @param font Font used for every line; if null, only the panel
             *        background is drawn (no text)
             * @param windowWidth current window width, used for panel size
             * @param windowHeight current window height, used for panel size
             */
            void Display(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight);

            /**
             * @brief Binds the console to the scene it should act on.
             * @param scene the currently running Scene (must not be null)
             */
            void GetSceneContext(Scene* scene);

            /**
             * @brief Gives the console a way to reach the running
             *        SceneManager (used by e.g. `load`, `show_scene_list`).
             * @param sceneManager SceneManager owned by the App
             */
            void SetSceneManager(Managers::SceneManager* sceneManager) { m_sceneManager = sceneManager; }

            /**
             * @brief Parses and runs a single console command line, e.g.
             *        "teleport_entity player 10 20".
             * @param command the raw command line as typed
             */
            void ExecuteCommand(const string& command);

            /**
             * @brief Routes input events to the console's input line.
             *
             * While the console is visible this consumes `TextInputEvent`
             * (typed characters) and `KeyPressedEvent` (Backspace/Return),
             * and swallows every other key so gameplay systems don't also
             * react to whatever is being typed. `F3` is never swallowed
             * here, so `UISystem` can always toggle visibility.
             * @param e GKC_Event
             */
            void OnEvent(Events::GKC_Event& e);

            /// @brief Flips whether the console panel is drawn and captures input.
            void ToggleVisibility() { m_isVisible = !m_isVisible; }
            [[nodiscard]] bool IsVisible() const { return m_isVisible; }

        private:
            /// @brief Runs every *Commands() handler; each ignores types it doesn't own.
            void CommandSwitch(const CommandWrapper& wrapper);

            /// @brief Resolves a bare, argument-less command (e.g. "help").
            CommandWrapper NoArgumentCommands(const string& command);
            /// @brief Resolves a command with a trailing argument string (e.g. "load myscene").
            CommandWrapper ArgumentCommands(const string& command, const string& args);

            void SceneCommands(const CommandWrapper& wrapper);
            void EntityCommands(const CommandWrapper& wrapper);
            void ScriptCommands(const CommandWrapper& wrapper);
            void AppCommands(const CommandWrapper& wrapper);
            void SoundCommands(const CommandWrapper& wrapper);
            void DebugCommands(const CommandWrapper& wrapper);

            /// @brief Clears all buffered log lines (`clear`).
            void ClearConsole();
            /// @brief Prints the list of available commands (`help`).
            void DisplayHelp(const string& type);
            /// @brief Prints the current frame rate, derived from Clock (`show_fps`).
            void ShowFPS();
            /// @brief Prints basic engine/display info (`show_debug_info`).
            void ShowDebugInformation();

            void InvalidArgumentError(const string& message);

            /// @brief Splits a command's argument string on whitespace.
            vector<string> GetListOfArguments(const string& args);
            /// @brief Maps a spdlog level to the color its line is drawn in.
            SDL_Color LevelToColor(spdlog::level::level_enum level);      
    };
}