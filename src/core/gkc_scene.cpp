#include <core/gkc_scene.h>
#include <filesys/gkc_filesys.h>
#include <render/gkc_window.h>

#include "core/gkc_clock.h"
#include "core/gkc_debugger.h"
#include "core/gkc_exception.h"
#include "core/gkc_logger.h"
#include "core/events/gkc_event.h"
#include "core/helpers/gkc_ecs_helper.h"
#include "core/managers/gkc_audio_man.h"
#include "core/managers/gkc_ecs_man.h"
#include "core/managers/gkc_texture_man.h"
#include "core/managers/gkc_window_man.h"
#include "core/systems/gkc_camera_system.h"
#include "core/systems/gkc_key.h"
#include "core/systems/gkc_mouse_system.h"
#include "core/systems/gkc_movement_system.h"
#include "core/systems/gkc_physics_system.h"
#include "core/systems/gkc_system.h"
#include "core/systems/gkc_ui_system.h"
#include "core/systems/gkc_window_system.h"
#include "filesys/gkc_writer.h"
#include "render/gkc_drawer.h"
#include "render/gkc_texture.h"

using namespace Galaktic::Core;
using namespace Galaktic::Filesystem;

Scene::Scene(const string& name, const DeviceInformation& device_information, const path& path)
    : m_sceneInfo({name, sizeof(Scene)} ) {

    GKC_ASSERT(!device_information.IsCorrupted(), "information from app is corrupted!");
    m_windowManager = new Managers::WindowManager();
    GKC_ASSERT(m_windowManager != nullptr, "Failed to create window manager!");
    m_window = make_shared<Render::Window>(name, device_information.width_,
        device_information.height_, Render::Window_Type::Resizable);

    m_windowManager->RegisterWindow(m_window);

    strcpy(Debug::Console::GetDebugInformation()->display_info_,
        Debug::Logger::GetDisplayInfo(device_information).c_str());

    GKC_ASSERT(m_window != nullptr, "Failed to create window!");

    // Managers Initialization
    m_registry = new ECS::Registry();
    m_systemList.reserve(GKC_SYSTEMS_COUNTER);

    const auto texturePath = path.filename() / GKC_TEXTURE_PATH;
    const auto soundPath = path.filename() / GKC_SOUND_PATH;

    m_ecsManager = new Managers::ECS_Manager(m_registry);
    m_textureManager = new Managers::TextureManager(texturePath, GKC_GET_RENDERER(m_window));
    m_audioManager = new Managers::AudioManager(soundPath);
    m_ecsHelper = new Helpers::ECS_Helper(*m_ecsManager);

    /* @todo Make a function to reset these defaults and change them when the file is read or
     *       change the full structure of these systems (use 1 please :v)
     */
    CreateCamera("Camera");
    CreatePlayer();
    auto& camera = m_ecsHelper->GetEntityByName("Camera");

    // Manage I/O Events used by other systems
    auto key_system = make_shared<Systems::KeySystem>();
    auto mouse_system = make_shared<Systems::MouseSystem>();

    // Engine-related systems
    auto movement_system = make_shared<Systems::MovementSystem>(*key_system);
    auto physics_system = make_shared<Systems::PhysicsSystem>();
    auto ui_system = make_shared<Systems::UISystem>(*key_system);
    auto window_system = make_shared<Systems::WindowSystem>();
    auto camera_system = make_shared<Systems::CameraSystem>(camera);
    camera_system->SetFollowEntity(2);


    // Systems added to manage events
    m_systemList.emplace("KeySystem", key_system);              // 0
    m_systemList.emplace("MouseSystem", mouse_system);          // 1
    m_systemList.emplace("MovementSystem",movement_system);     // 2
    m_systemList.emplace("PhysicsSystem", physics_system);      // 3
    m_systemList.emplace("UISystem", ui_system);                // 4
    m_systemList.emplace("WindowSystem",window_system);         // 5
    m_systemList.emplace("CameraSystem", camera_system);        // 6
    m_appPath = path.filename();

    GKC_ENGINE_INFO("Loading textures from... {0}", texturePath.string());
    GKC_ASSERT(m_registry != nullptr, "Failed to create entity manager!");
    GKC_ASSERT(m_ecsManager != nullptr, "Entity manager is NULL!");
    GKC_ASSERT(m_ecsHelper != nullptr, "Entity manager helper is NULL!");
    GKC_ASSERT(m_systemList.size() >= GKC_SYSTEMS_COUNTER, "system manager is NULL!");
    GKC_ASSERT(m_textureManager != nullptr, "Texture manager is NULL!");
    GKC_ASSERT(m_audioManager != nullptr, "Audio manager is NULL!");
}

void Scene::Run()  {
    // Allow events to be polled from window
    m_window->SetCallback(
    [this](Events::GKC_Event& event) {
            OnEvent(event);
        }
    );

    //@todo Make ECS System
    Clock::Init();
    double accumulator = 0.0;

    auto physics_system = m_systemList.find("PhysicsSystem")->second;
    auto movement_system = m_systemList.find("MovementSystem")->second;
    auto camera_system = m_systemList.find("CameraSystem")->second;

    // Used only in rendering
    auto camera_systemPtr = std::dynamic_pointer_cast<Systems::CameraSystem>(camera_system);

    GKC_ASSERT(camera_systemPtr != nullptr || camera_system != nullptr, "CameraSystem is NULL!");
    GKC_ASSERT(physics_system != nullptr, "physics_system is NULL!");
    GKC_ASSERT(movement_system != nullptr, "movement_system is NULL!");

    // @TODO Add a modifiable function to edit
    // Add Debug Information
    Debug::Console::SetRenderer(GKC_GET_RENDERER(m_window));
    strcpy(Debug::Console::GetDebugInformation()->engine_name_, Debug::Logger::GetEngineName().c_str());

    auto& player_transform = m_ecsHelper->GetEntityByName("Player").Get<ECS::TransformComponent>();

    #if GKC_DEBUG
        m_audioManager->PrintList();
        m_textureManager->PrintList();
    #endif

    while (m_isRunning) {
        // Timing
        Clock::Update();
        double delta_time = Clock::GetDeltaTime();
        delta_time = std::min(delta_time, 0.25);
        accumulator += delta_time;

        // Event Handling
        Debug::Console::GetDebugInformation()->ram_usage_ = Debug::GetGalakticRAMUsage();
        Debug::Console::GetDebugInformation()->fps_ = static_cast<float>(1 / delta_time);
        Debug::Console::GetDebugInformation()->x_coordinate_ = player_transform.location_.x;
        Debug::Console::GetDebugInformation()->y_coordinate_ = player_transform.location_.y;

        m_window->PollEvents();
        if (m_window->ShouldClose()) {
            Save();
            // @TODO Add something to tell if you really want to close the program
            Close(m_isRunning);
        }

        // Engine Physics management
        while (accumulator >= FIXED_DELTA_TIME) {
            // Physics System
            physics_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time));
            movement_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time));
            camera_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time)
                , m_window->GetWidth(), m_window->GetHeight());
            accumulator -= FIXED_DELTA_TIME;
        }

        // Drawer Functions
        m_window->Draw(GKC_GET_RENDERER(m_window));
        Render::Drawer::DrawEntities(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window),
            *camera_systemPtr);

        if (Debug::Console::GetIsActive()) {
            Debug::Console::CallConsole();
        }
        SDL_RenderPresent(GKC_GET_RENDERER(m_window));
        SDL_Delay(16); // Caps at ~60fps
    }
}

void Scene::Save() {
    FileWriter::WriteScene(m_appPath / path(m_sceneInfo.scene_name_ + ".gkscene")
        , *this, m_registry);
}

void Scene::OnEvent(Events::GKC_Event& event) {
    GKC_ASSERT(!m_systemList.empty(), "system manager is empty!");

    for (auto& system : m_systemList) {
        if (system.first == "PhysicsSystem" || system.first == "MovementSystem")
            continue;

        // Ignore delta-time based systems
        system.second->OnEvent(event);

        // Event handled continue
        if (event.m_handled)
            break;
    }
}

void Scene::Free() const {
    SDL_Quit();
    TTF_Quit();
    GKC_ENGINE_INFO("Closed libraries!");
    GKC_ENGINE_INFO("'{0}' scene closed", m_sceneInfo.scene_name_);
}

void Scene::CreateStaticObject(const string &name) {
    m_ecsHelper->CreateStaticObject(name);
}

void Scene::CreatePhysicsObject(const string& name) {
    m_ecsHelper->CreatePhysicsObject(name);
}

void Scene::CreateLightEntity(const string& name) {
    m_ecsHelper->CreateLightEntity(name);
}

void Scene::CreatePlayer() {
    m_ecsHelper->CreatePlayer("Player");
}

void Scene::CreateCamera(const string& name) {
    auto cameraComponent = ECS::CameraComponent();
    cameraComponent.active_ = true;
    m_ecsHelper->CreateCameraEntity(name);
    m_ecsHelper->ModifyEntity(name, cameraComponent);
}
void Scene::Close(bool& is_running) const {
    Free();
    is_running = false;
    exit(0);
}
