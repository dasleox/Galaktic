/*
  Galaktic Engine
  Copyright (C) 2025 SummerChip

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

namespace Galaktic::Debug {
    /**
     * @struct DebugInformation
     * @brief Saves all the debug information, shows when pressing F3
     */
    struct DebugInformation {
        size_t ram_usage_;
        size_t ram_available_;
        char engine_name_[64];
        char display_info_[64];
        float fps_;
        float x_coordinate_;
        float y_coordinate_;

        DebugInformation();
    };

    /**
     * @class Console
     * @brief Shows the debug information in the current scene
     */
    class Console {
        public:
            /**
             * @brief Calls the console and display bugs information
             */
            static void CallConsole();

            /**
             * @brief Displays simple debugging information about the scene
             * @deprecated This function was used in early versions to test spdlog formatting
             */
            static void CallSimpleConsole();
            static DebugInformation*& GetDebugInformation() { return m_info; }
            static bool& GetIsActive() { return m_isActive; }
            static void SetRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
        private:
            static DebugInformation* m_info;
            static SDL_Renderer* m_renderer;
            static bool m_isActive;
    };

    /**
     * @brief Inits all required libraries for Galaktic
     * @note This function needs to be called before using any library (For now only SDL requires this function)
     */
    extern void StartLibraries();

    /**
     * @brief Gets hardware information of the device
     * @return A DeviceInformation instance
     */
    extern Core::DeviceInformation GetDeviceInformation();
}

