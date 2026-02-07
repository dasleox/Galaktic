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
#include <core/gkc_scene.h>

namespace Galaktic::Core::Managers {
    class SceneManager; 
}

namespace Galaktic::Core {
    class ManagersWrapper;

    /**
     * @brief Main application class that initializes and runs the application.
     * @class App
     * 
     * This class is responsible for setting up the application environment,
     * managing scenes inside the project folder, and handling device information.
     * A SceneManager instance is created to manage scenes within the application,
     * can be accsessed using \c GetSceneManager()
     * 
     * @see gkc_scene_man.h for more information about SceneManager.
     */
    class App {
        public:
            /**
             * @param project_path Project folder path
             * @param title Title of the app
             */
            App(const path& project_path, const string& title);

            Managers::SceneManager*& GetSceneManager() { return m_sceneManager; }
        private:
            Managers::SceneManager* m_sceneManager;         // Scene Manager
            DeviceInformation m_deviceInfo;                 // Device Info
            unique_ptr<ManagersWrapper> m_managersWrapper;  // Managers Wrapper Pointer
            string m_appName;                               // App name

            /**
             * @brief Get the screen information and set the width and height of the built window
             */
            void ScreenStartup();
    };
}
