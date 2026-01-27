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
#include <core/systems/gkc_system.h>
#include "ecs/gkc_registry.h"

typedef Uint32 EntityID;
namespace Galaktic::Core::Helpers {
    class ECS_Helper;
}
namespace Galaktic::Core::Events {
    class GKC_Event;
}
namespace Galaktic::ECS {
    class Entity;
}
namespace Galaktic::Render {
    class Window;
}
namespace Galaktic::Core::Systems {
    typedef unordered_map<string, shared_ptr<BaseSystem>> System_List;
}

namespace Galaktic::Core::Managers {
    class WindowManager;
    class ECS_Manager;
    class TextureManager;
    class AudioManager;
}

namespace Galaktic::Core {

    /**
     * @struct SceneInformation
     * @brief Saves all the information about the scene
     */
    struct SceneInformation {
        string scene_name_;
        size_t scene_size_ = 0;
    };

    /**
     * @class Scene
     * @brief Scene containing objects, texts, etc...
     */
    class Scene {
        public:
            /**
             * @param name Name of the scene
             * @param device_information DeviceInformation instance
             * @param path Path of the app
             */
            Scene(const string& name, const DeviceInformation& device_information, const path& path);

            /**
             * @brief Runs the Scene.
             */
            void Run();

            void Save();

            /**
             * @brief Executes all events using the engine's systems
             * @param event GKC_Event
             */
            void OnEvent(Events::GKC_Event& event);

            /**
             * @brief Close the current scene
             * @param is_running Bool scene running
             */
            void Close(bool& is_running) const;

            /**
             * @brief Free all the resources from the scene
             */
            void Free() const;

            void CreateStaticObject(const string &name);

            void CreatePhysicsObject(const string &name);

            void CreateLightEntity(const string &name);

            ECS::Registry*& GetRegistry() { return m_registry; }
            Managers::ECS_Manager*& GetECSManager() { return m_ecsManager; }
            SceneInformation m_sceneInfo;
        private:
            bool m_isRunning = true;
            shared_ptr<Render::Window> m_window;
            Systems::System_List m_systemList;
            ECS::Registry* m_registry = nullptr;
            Managers::WindowManager* m_windowManager = nullptr;
            Managers::ECS_Manager* m_ecsManager = nullptr;
            Helpers::ECS_Helper* m_ecsHelper = nullptr;
            Managers::TextureManager* m_textureManager = nullptr;
            Managers::AudioManager* m_audioManager = nullptr;
            path m_appPath;
            void CreatePlayer();
            void CreateCamera(const string &name);
    };

    typedef unordered_map<string, unique_ptr<Scene>> Scene_List;
}


#define GKC_GSCENE_STRING(name) (name + ".gscene")