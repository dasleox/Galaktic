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

namespace Galaktic::Core {
    class Scene;
}

namespace Galaktic::Core::Managers {
    /**
     * @brief Manages all scenes
     *
     * Manages all scenes inside a list and associated folder (application/project)
     * folder, a DeviceInfo instance is required to initialize scenes, this class
     * can create, delete, and load scenes from files using a file format
     * with the extension .gkscene
     */
    class SceneManager {
        public:
            SceneManager(const path& folder, const DeviceInformation& info);

            /**
             * Creates a scene with the passed name
             * @param name scene's name
             */
            void CreateScene(const string& name);

            /**
             * Deletes a scene with the passed name
             * @param name scene's name
             */
            void DeleteScene(const string& name);

            /**
             * Loads a specific scene by its name inside the list,
             * and runs it
             * @param name 
             */
            void LoadSpecificScene(const string& name) const;

            /**
             * Loads a specific scene from a file (.gkscene), if the file
             * is loaded correctly, it is added to the scene list.
             * @param filepath 
             */
            void LoadSpecificSceneFromFile(const path& filepath);

            /**
             * Gets the scene pointer if it exists on the list
             * @param name 
             * @return unique_ptr<Scene> if the scene exists, nullptr otherwise
             */
            unique_ptr<Scene> GetScene(const string& name);
        private:
            DeviceInformation m_deviceInfo;
            path m_folder;
            unordered_map<string, unique_ptr<Scene>> m_sceneList;
    };
}