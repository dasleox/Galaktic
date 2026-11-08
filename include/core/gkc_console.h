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

        /**
         * @brief Initializes the diagnostic snapshot with its default values.
         */
        DebugInformation() = default;
    };

    namespace Managers 
    {
        class SceneManager;
    }

    /**
     * @enum CommandTypes
     * @brief Identifies the operation requested by a console command.
     */
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
        CopyEntity,

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
        SetMasterVolume,
        
        
        // Debug
        ShowFPSToggle,
        ShowWireframeToggle,
        ShowCollidersToggle,
        ShowDebugInformationToggle,
    };

    /**
     * @struct CommandWrapper
     * @brief Stores a parsed console command, its arguments and its operation type.
     */
    struct CommandWrapper   
    {
        /**
         * @brief Initializes a parsed command and its argument string.
         */
        CommandWrapper() {}
        /**
         * @brief Initializes a parsed command and its argument string.
         * @param command Console command name
         * @param args Arguments forwarded to the requested operation
         * @param type Engine type identifier used by this operation
         */
        CommandWrapper(const string& command, const string& args, CommandTypes type)
        {
            this->command = command;
            this->args = args;
            this->type = type;
        }

        /**
         * @brief Checks whether the parsed command has no recognized name or operation type.
         * @return true if the parsed command has no recognized name or operation type, false otherwise
         */
        constexpr bool IsEmpty() {
            return command.empty() || type == CommandTypes::Unknown; 
        }
        /**
         * @brief Checks whether the parsed command contains an argument string.
         * @return true if the parsed command contains an argument string, false otherwise
         */
        constexpr bool HasArguments()
        {
            return !args.empty();
        }
        string command = "";
        string args = "";
        CommandTypes type = CommandTypes::Unknown;
    };

    /**
     * @brief Removes ANSI escape sequences from a log line, and trims trailing newlines.
     * @param text 
     * @return string Cleaned log line
     */
    inline string CleanLogText(const string& text)
    {
        string out;
        out.reserve(text.size());

        for (size_t i = 0; i < text.size(); ++i)
        {
            if (text[i] == '\033' && i + 1 < text.size() && text[i + 1] == '[')
            {
                i += 2;
                while (i < text.size() && !std::isalpha(static_cast<unsigned char>(text[i]))) ++i;
                continue;
            }
            out += text[i];
        }

        while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
        return out;
    }

    /**
     * @class Console
     * @brief In-game developer console: displays engine log output and lets
     *        the player type commands that reach the scene/ECS/scripting/
     *        audio/config systems.
     */
    class Console {
        private:
            // Called by Display with native-pixel coordinates; may return early.
            /**
             * @brief Draws console content in the native-pixel coordinate system established by Display.
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param font Borrowed TTF_Font, which must outlive the text using it
             * @param windowWidth Window width used for panel layout
             * @param windowHeight Window height used for panel layout
             */
            void DisplayContents(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight);
            Scene* m_currentScene = nullptr;
            Managers::SceneManager* m_sceneManager = nullptr;
            unique_ptr<DebugInformation> m_pDebugInfo = nullptr;
            shared_ptr<Debug::ConsoleMutex_mt> m_sink = nullptr;

            bool m_cheatsToggle = false;
            bool m_isVisible = false;

            /// Text currently being typed into the console's input line.
            string m_inputBuffer;

            std::stack<string> m_commandHistory;

            static constexpr int m_wheelStep = 3;
            int m_scrollOffset = 0;     
            int m_visibleLines = 0;     
            int m_lastLineCount = 0; 
        public:
            /**
             * @brief Stores the shared log sink used to display console messages.
             * @param sink Shared console log sink
             */
            Console(shared_ptr<Debug::ConsoleMutex_mt> sink);

            /**
             * @brief Draws the console in native output pixels and restores the
             *        caller's renderer state. Shows recent errors while closed.
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
            /**
             * @brief Toggles whether the console panel and input line are visible.
             */
            void ToggleVisibility() { m_isVisible = !m_isVisible; }
            /**
             * @brief Checks whether the console panel is visible.
             * @return true if the console panel is visible, false otherwise
             */
            [[nodiscard]] bool IsVisible() const { return m_isVisible; }

            /**
                * @brief Scrolls the console's log output by a number of lines.
                * @param lines positive = scroll up, negative = scroll down
            */
            void ScrollBy(int lines);

            /// @brief Scrolls the console's log output to the bottom (most recent lines).
            /**
             * @brief Resets console scrolling to the newest log messages.
             */
            void ScrollToBottom() { m_scrollOffset = 0; }
            
        private:
            /// @brief Runs every *Commands() handler; each ignores types it doesn't own.
            /**
             * @brief Dispatches a parsed command to its command category handler.
             * @param wrapper Parsed console command and its arguments
             */
            void CommandSwitch(const CommandWrapper& wrapper);

            /// @brief Resolves a bare, argument-less command (e.g. "help").
            /**
             * @brief Classifies a console command that does not take arguments.
             * @param command Console command name
             */
            CommandWrapper NoArgumentCommands(const string& command);
            /// @brief Resolves a command with a trailing argument string (e.g. "load myscene").
            /**
             * @brief Classifies a console command and stores its argument string.
             * @param command Console command name
             * @param args Arguments forwarded to the requested operation
             */
            CommandWrapper ArgumentCommands(const string& command, const string& args);

            /**
             * @brief Executes scene-management console commands.
             * @param wrapper Parsed console command and its arguments
             */
            void SceneCommands(const CommandWrapper& wrapper);
            /**
             * @brief Executes entity-management console commands.
             * @param wrapper Parsed console command and its arguments
             */
            void EntityCommands(const CommandWrapper& wrapper);
            /**
             * @brief Executes script-management console commands.
             * @param wrapper Parsed console command and its arguments
             */
            void ScriptCommands(const CommandWrapper& wrapper);
            /**
             * @brief Executes application and console-control commands.
             * @param wrapper Parsed console command and its arguments
             */
            void AppCommands(const CommandWrapper& wrapper);
            /**
             * @brief Executes audio playback and volume console commands.
             * @param wrapper Parsed console command and its arguments
             */
            void SoundCommands(const CommandWrapper& wrapper);
            /**
             * @brief Executes diagnostic and rendering-overlay console commands.
             * @param wrapper Parsed console command and its arguments
             */
            void DebugCommands(const CommandWrapper& wrapper);

            /// @brief Clears all buffered log lines (`clear`).
            /**
             * @brief Clears the console's log sink.
             */
            void ClearConsole();
            /// @brief Prints the list of available commands (`help`).
            /**
             * @brief Logs help for the requested console command section.
             * @param type Engine type identifier used by this operation
             */
            void DisplayHelp(const string& type);
            /// @brief Prints the current frame rate, derived from Clock (`show_fps`).
            /**
             * @brief Logs frames per second calculated from the last clock delta.
             */
            void ShowFPS();
            /// @brief Prints basic engine/display info (`show_debug_info`).
            /**
             * @brief Logs the engine's diagnostic information.
             */
            void ShowDebugInformation();

            /**
             * @brief Logs a console argument error.
             * @param message Diagnostic message
             */
            void InvalidArgumentError(const string& message);

            /// @brief Splits a command's argument string on whitespace.
            /**
             * @brief Splits a console argument string into individual arguments.
             * @param args Arguments forwarded to the requested operation
             */
            vector<string> GetListOfArguments(const string& args);
            /// @brief Maps a spdlog level to the color its line is drawn in.
            /**
             * @brief Returns the console text color associated with a log severity.
             * @param level Log severity
             */
            SDL_Color LevelToColor(spdlog::level::level_enum level);      

            /**
             * @brief Draws recent error messages when a usable font and log sink are available.
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param font Borrowed TTF_Font, which must outlive the text using it
             */
            void DisplayErrors(SDL_Renderer* renderer, TTF_Font* font);
    };
}
