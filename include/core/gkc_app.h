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
#include <core/gkc_scene.h>

namespace Galaktic::Core {
    /**
     * @brief Main application class that initializes and runs the application.
     * @class App
     */
    class App {
        public:
            App(const path& project_path, const string& title);

            /**
             * @brief Loads the specific scene inside the App's directory
             *        structure, this will search up the name inside the Scene_List
             *        in this class (map)
             * @param scene_name The name of the scene to load
             */
            void LoadSpecificScene(const string &scene_name);
        private:
            Scene_List m_sceneList;             // App Scene List
            DeviceInformation m_deviceInfo;     // Device Info
            string m_appName;                   // App name

            /**
             * @brief Get the screen information and set the width and height of the built window
             */
            void ScreenStartup();
    };
}
