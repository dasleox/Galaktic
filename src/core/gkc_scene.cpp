#include <core/gkc_scene.h>
#include <render/gkc_window.h>
#include "core/gkc_clock.h"
#include "core/gkc_debugger.h"
#include "core/gkc_logger.h"
#include "core/events/gkc_event.h"
#include "core/helpers/gkc_ecs_helper.h"
#include "core/managers/gkc_audio_man.h"
#include "core/managers/gkc_ecs_man.h"
#include "core/managers/gkc_texture_man.h"
#include "core/managers/gkc_script_man.h"
#include "core/managers/gkc_window_man.h"
#include "core/systems/gkc_camera_system.h"
#include "core/systems/gkc_key.h"
#include "core/systems/gkc_mouse_system.h"
#include "core/systems/gkc_movement_system.h"
#include "core/systems/gkc_physics_system.h"
#include "core/systems/gkc_system.h"
#include "core/systems/gkc_ui_system.h"
#include "core/systems/gkc_window_system.h"
#include "core/systems/gkc_ecs_event_system.h"
#include "core/systems/gkc_script_system.h"
#include "filesys/gkc_writer.h"
#include "render/gkc_drawer.h"
#include "script/gkc_script.h"
#include "core/helpers/gkc_texture_helper.h"
#include "core/helpers/gkc_animation_helper.h" 
#include "core/managers/gkc_animation_man.h"

using namespace Galaktic::Core;
using namespace Galaktic::Filesystem;

Scene::Scene(const string& name, ManagersWrapper* wrapper, const DeviceInformation& device_information, const path& path)
    : m_sceneInfo({name, sizeof(Scene)}), m_managerWrapper(wrapper) {

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
    m_ecsManager = new Managers::ECS_Manager(m_registry);
    m_ecsHelper = new Helpers::ECS_Helper(*m_ecsManager);
    m_textureHelper = new Helpers::TextureHelper(*m_ecsManager);
    m_animationHelper = new Helpers::AnimationHelper(*m_ecsManager);
    m_managerWrapper->m_textureManager->CreateMissingTexture(GKC_GET_RENDERER(m_window));
    
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
    auto entity_event_system = make_shared<Systems::ECS_EventSystem>(*m_ecsManager);
    camera_system->SetFollowEntity(2);


    // Systems added to manage events
    m_systemList.emplace("KeySystem", key_system);                  // 0
    m_systemList.emplace("MouseSystem", mouse_system);              // 1
    m_systemList.emplace("MovementSystem",movement_system);         // 2
    m_systemList.emplace("PhysicsSystem", physics_system);          // 3
    m_systemList.emplace("UISystem", ui_system);                    // 4
    m_systemList.emplace("WindowSystem",window_system);             // 5
    m_systemList.emplace("CameraSystem", camera_system);            // 6
    m_systemList.emplace("EntityEventSystem", entity_event_system); // 7
    m_appPath = path.filename();
    
    GKC_RELEASE_ASSERT(m_registry != nullptr, "Failed to create entity manager!");
    GKC_RELEASE_ASSERT(m_ecsManager != nullptr, "Entity manager is NULL!");
    GKC_RELEASE_ASSERT(m_ecsHelper != nullptr, "Entity manager helper is NULL!");
    GKC_RELEASE_ASSERT(m_systemList.size() >= GKC_SYSTEMS_COUNTER
        || !m_systemList.empty(), "system manager is NULL!");
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
    auto ecsEventSystem = m_systemList.find("EntityEventSystem")->second;

    // Used only in rendering
    auto camera_systemPtr = std::dynamic_pointer_cast<Systems::CameraSystem>(camera_system);

    GKC_RELEASE_ASSERT(camera_systemPtr != nullptr || camera_system != nullptr, "CameraSystem is NULL!");
    GKC_RELEASE_ASSERT(physics_system != nullptr, "physics_system is NULL!");
    GKC_RELEASE_ASSERT(movement_system != nullptr, "movement_system is NULL!");
    GKC_RELEASE_ASSERT(ecsEventSystem != nullptr, "entity_event_system is NULL!");

    // @TODO Add a modifiable function to edit
    // Add Debug Information
    Debug::Console::SetRenderer(GKC_GET_RENDERER(m_window));
    strcpy(Debug::Console::GetDebugInformation()->engine_name_, Debug::Logger::GetEngineName().c_str());

    m_managerWrapper->m_textureManager->LoadAllTextures(GKC_GET_RENDERER(m_window));
    m_managerWrapper->m_animationManager->LoadAllAnimations(GKC_GET_RENDERER(m_window));
    
    auto& player = m_ecsHelper->GetEntityByName("Player");
    auto& player_transform = player.Get<ECS::TransformComponent>();

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
            Close();
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
        m_managerWrapper->m_animationManager->UpdateAll(delta_time);

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

void Scene::CopyEntityList(ECS::Entity_List list) {
    m_ecsManager->GetEntityList() = list;
}

void Scene::CreatePlayer() {
    m_ecsHelper->CreatePlayer("Player");
}

void Scene::CreateCamera(const string& name) {
    auto cameraComponent = ECS::CameraComponent();
    cameraComponent.isActive_ = true;
    m_ecsHelper->CreateCameraEntity(name);
    m_ecsHelper->ModifyEntity(name, cameraComponent);
}
void Scene::Close() {
    Free();
    m_isRunning = false;
    exit(0);
}
