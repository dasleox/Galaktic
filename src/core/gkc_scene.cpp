#include <core/gkc_scene.h>
#include <filesys/gkc_filesys.h>
#include <render/gkc_window.h>

#include "core/gkc_clock.h"
#include "core/gkc_debugger.h"
#include "core/gkc_logger.h"
#include "core/events/gkc_event.h"
#include "core/managers/gkc_ecs_man.h"
#include "core/systems/gkc_key.h"
#include "core/systems/gkc_mouse_system.h"
#include "core/systems/gkc_movement_system.h"
#include "core/systems/gkc_physics_system.h"
#include "core/systems/gkc_system.h"
#include "core/systems/gkc_ui_system.h"
#include "core/systems/gkc_window_system.h"
#include "render/gkc_drawer.h"

using namespace Galaktic::Core;
using namespace Galaktic::Filesystem;

Scene::Scene(const string& name, const DeviceInformation& device_information, const path& path)
    : m_sceneInfo({name, sizeof(Scene)} ) {

    GKC_ASSERT(!device_information.IsCorrupted(), "information from app is corrupted!");
    m_window = make_shared<Render::Window>(name, device_information.width_,
        device_information.height_, Render::Window_Type::Resizable);
    strcpy(Debug::Console::GetDebugInformation()->display_info_,
        Debug::Logger::GetDisplayInfo(device_information).c_str());

    m_registry = new ECS::Registry();

    GKC_ASSERT(m_window != nullptr, "Failed to create window!");
    m_system_manager.reserve(GKC_SYSTEMS_COUNTER);

    // Manage I/O Events used by other systems
    auto key_system = make_shared<Systems::KeySystem>();
    auto mouse_system = make_shared<Systems::MouseSystem>();

    // Engine-related systems
    auto movement_system = make_shared<Systems::MovementSystem>(*key_system);
    auto physics_system = make_shared<Systems::PhysicsSystem>();
    auto ui_system = make_shared<Systems::UISystem>(*key_system);
    auto window_system = make_shared<Systems::WindowSystem>();

    // Systems added to manage events
    m_system_manager.emplace("KeySystem", key_system);              // 0
    m_system_manager.emplace("MouseSystem", mouse_system);          // 1
    m_system_manager.emplace("MovementSystem",movement_system);     // 2
    m_system_manager.emplace("PhysicsSystem", physics_system);      // 3
    m_system_manager.emplace("UISystem", ui_system);                // 4
    m_system_manager.emplace("WindowSystem",window_system);         // 5

    m_ecsManager = new Managers::ECS_Manager(m_registry);

    GKC_ASSERT(m_registry != nullptr, "Failed to create entity manager!");
    GKC_ASSERT(m_ecsManager != nullptr, "Entity manager is NULL!");
    GKC_ASSERT(m_system_manager.size() >= GKC_SYSTEMS_COUNTER, "system manager is NULL");
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

    auto physics_system = m_system_manager.find("PhysicsSystem")->second;
    auto movement_system = m_system_manager.find("MovementSystem")->second;

    GKC_ASSERT(physics_system != nullptr, "physics_system is NULL!");
    GKC_ASSERT(movement_system != nullptr, "movement_system is NULL!");

    // Add Debug Information
    Debug::Console::SetRenderer(GKC_GET_RENDERER(m_window));
    strcpy(Debug::Console::GetDebugInformation()->engine_name_, Debug::Logger::GetEngineName().c_str());

    // Add a default player
    m_ecsManager->CreateEntity<ECS::PlayerTag>("Player");
    ECS::TransformComponent vec; vec.size_ = {50.f, 50.f};
    m_ecsManager->AddComponentToEntity<ECS::TransformComponent>(1, vec);
    ECS::SpeedComponent speed;
    m_ecsManager->AddComponentToEntity<ECS::SpeedComponent>(1, speed);
    ECS::ColorComponent color;
    m_ecsManager->AddComponentToEntity<ECS::ColorComponent>(1, color);

    GKC_ENGINE_INFO("Running Scene {}", m_sceneInfo.scene_name_);

    auto& player_transform = m_ecsManager->GetEntityList().at(1).Get<ECS::TransformComponent>();


    while (m_isRunning) {
        // Timing
        Clock::Update();
        double delta_time = Clock::GetDeltaTime();
        delta_time = std::min(delta_time, 0.25);
        accumulator += delta_time;

        // Event Handling
        Debug::Console::GetDebugInformation()->ram_usage_ = Debug::GetGalakticRAMUsage();
        Debug::Console::GetDebugInformation()->fps_ = 1 / delta_time;
        Debug::Console::GetDebugInformation()->x_coordinate_ = player_transform.location_.x;
        Debug::Console::GetDebugInformation()->y_coordinate_ = player_transform.location_.y;

        m_window->PollEvents();

        // Engine Physics management
        while (accumulator >= FIXED_DELTA_TIME) {
            // Physics System
            physics_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time));
            movement_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time));
            accumulator -= FIXED_DELTA_TIME;
        }

        // Drawer Functions
        m_window->Draw(GKC_GET_RENDERER(m_window));
        if (Debug::Console::GetIsActive()) {
            Debug::Console::CallConsole();
        }

        Render::Drawer::DrawEntities(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window));
        SDL_RenderPresent(GKC_GET_RENDERER(m_window));
        SDL_Delay(16);
    }
}

void Scene::OnEvent(Events::GKC_Event& event) {
    GKC_ASSERT(!m_system_manager.empty(), "system manager is empty!");

    for (auto& system : m_system_manager) {
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

void Scene::Close(bool& is_running) const {
    Free();
    is_running = false;
    exit(0);
}