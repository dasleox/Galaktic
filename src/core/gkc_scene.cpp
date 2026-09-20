#include <core/gkc_scene.h>
#include <render/gkc_window.h>
#include <core/gkc_app.h>
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
#include <core/gkc_console.h>
#include "core/systems/gkc_ui_system.h"
#include "filesys/gkc_filesys.h"
#include "core/systems/gkc_window_system.h"
#include "core/systems/gkc_ecs_event_system.h"
#include "core/systems/gkc_script_system.h"
#include "filesys/gkc_writer.h"
#include "render/gkc_drawer.h"
#include "script/gkc_script.h"
#include "core/helpers/gkc_texture_helper.h"
#include "core/helpers/gkc_animation_helper.h" 
#include "core/managers/gkc_animation_man.h"
#include "script/gkc_library.h"
#include "ecs/gkc_component_registry.h"
#include "render/gkc_rpixels.h"
#include <core/helpers/gkc_rpixels_helper.h>

#include <numeric>
#include <random>

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


    GKC_ASSERT(m_window != nullptr, "Failed to create window!");

    // Managers Initialization
    m_registry = new ECS::Registry();
    m_systemList.reserve(GKC_SYSTEMS_COUNTER);
    m_ecsManager = new Managers::ECS_Manager(m_registry);
    m_ecsHelper = new Helpers::ECS_Helper(m_ecsManager);
    m_textureHelper = new Helpers::TextureHelper(*m_ecsManager);
    m_animationHelper = new Helpers::AnimationHelper(*m_ecsManager);
    m_rPixelsHelper = new Helpers::RPixelsHelper(*m_ecsManager);

    m_managerWrapper->m_textureManager->CreateMissingTexture(GKC_GET_RENDERER(m_window));
    
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

    //@FIX ME use following only a camera
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
    m_appPath = path;
    
    GKC_RELEASE_ASSERT(m_registry != nullptr, "Failed to create entity manager!");
    GKC_RELEASE_ASSERT(m_ecsManager != nullptr, "Entity manager is NULL!");
    GKC_RELEASE_ASSERT(m_ecsHelper != nullptr, "Entity manager helper is NULL!");
    GKC_RELEASE_ASSERT(m_systemList.size() >= GKC_SYSTEMS_COUNTER
        || !m_systemList.empty(), "system manager is NULL!");

    auto sceneScriptFolderPath = m_appPath / GKC_SCRIPT_PATH / "local" / name;
    if(!Filesystem::CheckDirectory(sceneScriptFolderPath)) {
        Filesystem::CreateFolder(sceneScriptFolderPath);
        
    }
}

Scene::~Scene() {
    auto& entityList = m_ecsManager->GetEntityList();
    vector<EntityID> entityIds;
    for (auto& [id, entity] : entityList) {
        entityIds.push_back(id);
    }
    for (auto id : entityIds) {
        m_ecsManager->DeleteEntity(id);
    }
    
    ECS::ComponentRegistry::Clear();
    delete m_textureHelper;
    delete m_animationHelper;
    delete m_ecsHelper;
    delete m_rPixelsHelper;
    delete m_ecsManager;
    delete m_registry;
    delete m_windowManager;

    Script::LuaGalaktic::Shutdown();
}

void Scene::Run()  {
    m_managerWrapper->m_textureManager->LoadAllAssets(m_window->GetRenderer());
    m_managerWrapper->m_animationManager->LoadAllAssets(m_window->GetRenderer());

    // Allow events to be polled from window
    m_window->SetCallback(
    [this](Events::GKC_Event& event) {
            OnEvent(event);
        }
    );

    auto console = Core::App::GetConsole();
    auto fontPath = path(m_appPath / "assets" / "fonts" / "minecraft.ttf");
    TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), 12.f);
    if(font == nullptr)
    {
        GKC_ENGINE_ERROR("Failed to open font");
        exit(0);
    }

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


    
    
    auto& player = m_ecsHelper->GetEntityByName("Player");
    auto& player_transform = player.Get<ECS::TransformComponent>();

    /**@bug When creating an object the X and Y coordinates of the player are filled with gargabe and set to 0 after clicking */
    
    ECS::Entity myObject = m_ecsHelper->CreateStaticObject("myObject");

    m_textureHelper->SetTextureToEntity(player.GetID(), "cat.png");
    m_textureHelper->SetTextureToEntity(myObject.GetID(), "cat.png");

    SDL_Color mainColor = {123, 42, 11, 255};
    auto group = Render::PixelColoredGroupInfo("group", mainColor, player.GetID(), *m_ecsManager);

    Render::PixelMapping_List pixelMap = []() {
        std::vector<std::vector<bool>> map(32, std::vector<bool>(32, false));

        // 20% of 1024 = ~204 true values
        const int total = 32 * 32;
        const int trueCount = static_cast<int>(total * 0.2f);

        // Create a flat list of indices, shuffle, and pick the first trueCount
        std::vector<int> indices(total);
        std::iota(indices.begin(), indices.end(), 0);

        std::mt19937 rng(std::random_device{}());
        std::shuffle(indices.begin(), indices.end(), rng);

        for (int i = 0; i < trueCount; ++i) {
            int row = indices[i] / 32;
            int col = indices[i] % 32;
            map[row][col] = true;
        }

        return map;
    }();

    group.AddEntityToGroup(myObject.GetID());
    group.SetSelectedPixels(pixelMap);
    group.ApplyColorToEntities();

    while (m_isRunning) 
    {
        if(m_bPaused)
        {
            break;
        }
        // Timing
        Clock::Update();
        double delta_time = Clock::GetDeltaTime();
        delta_time = std::min(delta_time, 0.25);
        accumulator += delta_time;

        // Event Handling

        m_window->PollEvents();
        if (m_window->ShouldClose()) {
            Save();
            Close();
        }

        // Engine Physics management
        while (accumulator >= FIXED_DELTA_TIME) {
            // Physics System
            // @todo Remake this class and how NOW it behaves to new entities types
            ///physics_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time));
            movement_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time));
            camera_system->Update(m_ecsManager->GetEntityList(), static_cast<float>(delta_time)
                , m_window->GetWidth(), m_window->GetHeight());
            accumulator -= FIXED_DELTA_TIME;
        }

        // Drawer Functions
        m_window->Draw(GKC_GET_RENDERER(m_window));
        Render::Drawer::DrawEntities(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window),
            *camera_systemPtr);
        
        m_rPixelsHelper->DrawAllGroups();
        m_managerWrapper->m_animationManager->UpdateAll(delta_time);

        if(m_bShowWireframe)
        {
            Render::Drawer::DrawWireframes(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window),
                *camera_systemPtr);
        }

        if(m_bShowColliders)
        {
            Render::Drawer::DrawColliders(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window),
                *camera_systemPtr);
        }
        
        console->Display(GKC_GET_RENDERER(m_window), font, m_window->GetWidth(), m_window->GetHeight());

        SDL_RenderPresent(GKC_GET_RENDERER(m_window));
        SDL_Delay(16); // Caps at ~60fps
    }
}

void Scene::Save() {
    FileWriter::WriteScene(m_appPath / path(m_sceneInfo.scene_name_ + ".gkscene")
        , *this, m_registry);
}

void Scene::Pause() 
{
    !m_bPaused;
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
    cameraComponent.isActive = true;
    m_ecsHelper->CreateCameraEntity(name);
    m_ecsHelper->ModifyEntity(name, cameraComponent);
}
void Scene::Close() {
    m_isRunning = false;
}

void Scene::SetFullscreen()
{
    m_window->SetFullScreen();
}