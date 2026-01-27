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

namespace Galaktic::Core::Managers {
    class SceneManager {
        public:
            explicit SceneManager(const path& folder, const DeviceInformation& info);

            void CreateScene(const string& name);
            /**
             * @brief Loads the specific scene inside the App's directory
             *        structure, this will search up the name inside the Scene_List
             *        in this class (map)
             * @param name The name of the scene to load
             */
            void LoadSpecificScene(const string& name) const;
            void LoadSpecificSceneFromFile(const path& filepath);

            unique_ptr<Scene> GetScene(const string& name);

            Scene_List& GetSceneList() { return m_sceneList; }
        private:
            Scene_List m_sceneList;
            const DeviceInformation & m_deviceInfo;
            path m_folder;
    };
}