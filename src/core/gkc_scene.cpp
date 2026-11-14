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
#include "config/gkc_config_setter.h"
#include "core/helpers/gkc_texture_helper.h"
#include "core/helpers/gkc_animation_helper.h" 
#include "core/managers/gkc_animation_man.h"
#include "script/gkc_library.h"
#include "ecs/gkc_component_registry.h"
#include "render/gkc_rpixels.h"
#include <core/helpers/gkc_rpixels_helper.h>
#include "core/managers/gkc_font_man.h"
#include <core/managers/gkc_config_manager.h>
#include <config/gkc_config_values.h>

#include <algorithm>
#include <cmath>

using std::make_shared;
using std::filesystem::path;
using std::shared_ptr;
using std::string;
using std::vector;

using namespace Galaktic::Core;
using namespace Galaktic::Filesystem;

namespace 
{
    auto CreateFirstWindow = [](const Galaktic::Core::DeviceInformation& device_information, const std::filesystem::path& app_path) {
        const auto validDimension = [](int value) { return value >= 0 && value < 16000; };
        const int width = Galaktic::Config::GetValueOrDefault<int>("general.cfg", "window_width", 0, validDimension);
        const int height = Galaktic::Config::GetValueOrDefault<int>("general.cfg", "window_height", 0, validDimension);
        const auto title = Galaktic::Config::GetValueOrDefault<std::string>("general.cfg", "window_title", "Galaktic",
            [](const string& value) { return !value.empty() && value.find('\0') == string::npos; });
        auto type = Galaktic::Render::Window_Type::Resizable;
        const auto configuredType = Galaktic::Config::GetValueOrDefault<std::string>("general.cfg", "window_type", "Resizable");
        if(configuredType == "Fullscreen") type = Galaktic::Render::Window_Type::Fullscreen;
        else if(configuredType == "Borderless") type = Galaktic::Render::Window_Type::Borderless;
        else if(configuredType == "Transparent") type = Galaktic::Render::Window_Type::Transparent;
        else if(configuredType == "Hidden") type = Galaktic::Render::Window_Type::Hidden;
        else if(configuredType == "OpenGL") type = Galaktic::Render::Window_Type::OpenGL;
        auto window = make_shared<Galaktic::Render::Window>(title, width > 0 ? static_cast<Uint32>(width) : device_information.width_,
            height > 0 ? static_cast<Uint32>(height) : device_information.height_, type, width == 0 && height == 0);
        
        return window;
    };
}

Scene::Scene(const string& name, ManagersWrapper* wrapper, const DeviceInformation& device_information, const path& path)
    : m_sceneInfo({name, sizeof(Scene)}), m_managerWrapper(wrapper) {

        
    GKC_ASSERT(!device_information.IsCorrupted(), "information from app is corrupted!");
    if (Managers::WindowManager::GetWindowList().empty()) 
    {
        m_window = CreateFirstWindow(device_information, path);
        GKC_RELEASE_ASSERT(m_window != nullptr, "Failed to create the first window for the scene!");
        m_window->SetFullScreen(Config::GetValueOrDefault<bool>("general.cfg", "window_fullscreen", false));
        m_window->SetPixelResolution(320, 180);
        Managers::WindowManager::RegisterWindow(m_window);
    } 
    else 
    {
        m_window = Managers::WindowManager::GetWindow(1);
        m_window->SetTitle(name);
        m_window->SetPixelResolution(320, 180);
    }

    m_managerWrapper->m_textureManager->CreateMissingTexture(GKC_GET_RENDERER(m_window));

    m_appPath = path;
    m_bPaused = Config::GetValueOrDefault<bool>("general.cfg", "scene_paused", m_bPaused);
    m_bShowWireframe = Config::GetValueOrDefault<bool>("general.cfg", "show_wireframes", m_bShowWireframe);
    m_bShowColliders = Config::GetValueOrDefault<bool>("general.cfg", "show_colliders", m_bShowColliders);

    InitializeECS();
    InitializeHelpers();

    CreateCamera("Camera");
    CreatePlayer();

    InitializeSystems();
    EnsureSceneScriptFolder(name);
}

void Scene::InitializeECS() {
    m_registry = new ECS::Registry();
    m_systemList.reserve(GKC_SYSTEMS_COUNTER);
    m_ecsManager = new Managers::ECS_Manager(m_registry);

    GKC_RELEASE_ASSERT(m_registry != nullptr, "Failed to create entity registry!");
    GKC_RELEASE_ASSERT(m_ecsManager != nullptr, "Entity manager is NULL!");
}

void Scene::InitializeHelpers() {
    m_ecsHelper = new Helpers::ECS_Helper(m_ecsManager);
    m_textureHelper = new Helpers::TextureHelper(*m_ecsManager);
    m_animationHelper = new Helpers::AnimationHelper(*m_ecsManager);
    m_rPixelsHelper = new Helpers::RPixelsHelper(*m_ecsManager);

    GKC_RELEASE_ASSERT(m_ecsHelper != nullptr, "Entity manager helper is NULL!");
}

void Scene::InitializeSystems() {
    auto& camera = m_ecsHelper->GetEntityByName("Camera");

    // Input systems the other systems below read state from
    auto keySystem = make_shared<Systems::KeySystem>();
    auto mouseSystem = make_shared<Systems::MouseSystem>();

    auto movementSystem = make_shared<Systems::MovementSystem>(*keySystem);
    auto physicsSystem = make_shared<Systems::PhysicsSystem>(
        Config::GetValueOrDefault<float>("general.cfg", "gravity", 0.f));
    auto uiSystem = make_shared<Systems::UISystem>(*keySystem);
    auto windowSystem = make_shared<Systems::WindowSystem>();
    auto cameraSystem = make_shared<Systems::CameraSystem>(camera);
    auto entityEventSystem = make_shared<Systems::ECS_EventSystem>(*m_ecsManager);
        
    cameraSystem->FollowPlayer(m_ecsManager->GetEntityList());

    m_systemList.emplace("KeySystem", keySystem);
    m_systemList.emplace("MouseSystem", mouseSystem);
    m_systemList.emplace("MovementSystem", movementSystem);
    m_systemList.emplace("PhysicsSystem", physicsSystem);
    m_systemList.emplace("UISystem", uiSystem);
    m_systemList.emplace("WindowSystem", windowSystem);
    m_systemList.emplace("CameraSystem", cameraSystem);
    m_systemList.emplace("EntityEventSystem", entityEventSystem);

    GKC_RELEASE_ASSERT(m_systemList.size() >= GKC_SYSTEMS_COUNTER, "Not all engine systems were registered!");
}

void Scene::EnsureSceneScriptFolder(const string& name) {
    auto sceneScriptFolderPath = Config::GetDirectoryOrDefault("script_path", m_appPath, GKC_SCRIPT_PATH) / "local" / name;
    std::error_code error;
    std::filesystem::create_directories(sceneScriptFolderPath, error);
    if(error) GKC_ENGINE_WARNING("Could not create scene script folder '{}': {}", sceneScriptFolderPath.string(), error.message());
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

    Managers::TextureManager::DestroyMissingTexture();
    Script::LuaGalaktic::Shutdown();
}

void Scene::ReloadAssets(double deltaTime)
{
    m_hotReloadTimer += deltaTime;
    if (m_hotReloadTimer < 5.0)
        return;

    m_hotReloadTimer = 0.0;

    m_managerWrapper->m_textureManager->CheckForReloadAssets(GKC_GET_RENDERER(m_window));
    m_managerWrapper->m_animationManager->CheckForReloadAssets(GKC_GET_RENDERER(m_window));
    m_managerWrapper->m_audioManager->ReloadAudioFiles();
}

void Scene::Run()  {
    Managers::AudioManager::SetPaused(m_bPaused);
    LoadRunAssets();

    // Route SDL events polled by the window into the scene's event pipeline
    m_window->SetCallback(
        [this](Events::GKC_Event& event) {
            OnEvent(event);
        }
    );

    auto console = Core::App::GetConsole();
    console->GetSceneContext(this); 


    auto fontPath = Config::GetDirectoryOrDefault("font_path", m_appPath, "assets/fonts") / "console_font.ttf";
    TTF_Font* consoleFont = TTF_OpenFont(fontPath.string().c_str(), 13.f);
    if (consoleFont == nullptr)
    {
        GKC_ENGINE_WARNING("Failed to open console font '{}', falling back to the engine's default font.", fontPath.string());
        consoleFont = Managers::FontManager::GetDefaultFont();
    }
    if (consoleFont == nullptr)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound, "No usable font is available; the console will render without text.");
    }

    Clock::Init();
    double accumulator = 0.0;

    auto physicsSystem     = m_systemList.find("PhysicsSystem")->second;
    auto movementSystem    = m_systemList.find("MovementSystem")->second;
    auto cameraSystem      = std::dynamic_pointer_cast<Systems::CameraSystem>(
        m_systemList.find("CameraSystem")->second);
    auto entityEventSystem = m_systemList.find("EntityEventSystem")->second;

    GKC_RELEASE_ASSERT(cameraSystem != nullptr, "CameraSystem is NULL!");
    GKC_RELEASE_ASSERT(physicsSystem != nullptr, "PhysicsSystem is NULL!");
    GKC_RELEASE_ASSERT(movementSystem != nullptr, "MovementSystem is NULL!");
    GKC_RELEASE_ASSERT(entityEventSystem != nullptr, "EntityEventSystem is NULL!");

    SpawnDebugEntities();

    while (m_isRunning) 
    {
        Clock::Update();
        double deltaTime = std::min(Clock::GetDeltaTime(), 0.25);
        if (!m_bPaused) accumulator += deltaTime;
        else accumulator = 0.0;

        ReloadAssets(deltaTime);

        m_window->PollEvents();
        if (m_window->ShouldClose()) {
            Save();
            Close();
        }

        if (!m_bPaused)
        {
            while (accumulator >= FIXED_DELTA_TIME) {
                FixedUpdate(static_cast<float>(FIXED_DELTA_TIME), movementSystem, cameraSystem);
                accumulator -= FIXED_DELTA_TIME;
            }
        }
        
        RenderFrame(static_cast<float>(deltaTime), consoleFont, console, cameraSystem);
        SDL_Delay(16); // Caps the frame rate near 60 FPS
    }
}

void Scene::LoadRunAssets() {
    m_managerWrapper->m_textureManager->LoadAllAssets(m_window->GetRenderer());
    m_managerWrapper->m_animationManager->LoadAllAssets(m_window->GetRenderer());
}

TTF_Font* Scene::OpenConsoleFont() {
    auto fontPath = Config::GetDirectoryOrDefault("font_path", m_appPath, "assets/fonts") / "galaktic_font.ttf";
    TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), 12.f);
    if (font == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::AssetNotLoaded, "Failed to open the debug console font at '{}'", fontPath.string());
    }
    return font;
}

void Scene::SpawnDebugEntities() {
    // This development sandbox intentionally uses a small logical resolution.
    SetPixelResolution(320, 180, false);
    SetAmbientLight(0.15f);

    auto& player = m_ecsHelper->GetEntityByName("Player");
    auto& playerTransform = player.Get<ECS::TransformComponent>();
    playerTransform.location = {150.f, 92.f};
    playerTransform.size = {24.f, 24.f};
    playerTransform.zIndex = 2;
    player.Get<ECS::CollisionComponent>().collisionBox = playerTransform.size;
    m_textureHelper->SetTextureToEntity(player.GetID(), "cat.png");
    m_ecsManager->AddComponentToEntity<ECS::ShadowComponent>(player.GetID());


    auto floor = m_ecsHelper->CreateStaticObject("ShadowDemoFloor");
    auto& floorTransform = floor.Get<ECS::TransformComponent>();
    floorTransform.location = {-1024.f, -1024.f};
    floorTransform.size = {2048.f, 2048.f};
    floorTransform.zIndex = -10;
    floor.Get<ECS::ColorComponent>().color = {210, 210, 200, 255};
    floor.Get<ECS::CollisionComponent>().collidable = false;
    // No ShadowComponent: the floor receives lighting without blocking it.

    auto wall = m_ecsHelper->CreateStaticObject("ShadowDemoWall");
    auto& wallTransform = wall.Get<ECS::TransformComponent>();
    wallTransform.location = {106.f, 52.f};
    wallTransform.size = {12.f, 48.f};
    wallTransform.zIndex = 1;
    wall.Get<ECS::CollisionComponent>().collisionBox = wallTransform.size;
    wall.Get<ECS::ColorComponent>().color = {170, 120, 65, 255};
    m_ecsManager->AddComponentToEntity<ECS::ShadowComponent>(wall.GetID());

    auto rotated = m_ecsHelper->CreateStaticObject("ShadowDemoRotatedBlock");
    auto& rotatedTransform = rotated.Get<ECS::TransformComponent>();
    rotatedTransform.location = {212.f, 105.f};
    rotatedTransform.size = {26.f, 12.f};
    rotatedTransform.rotation = 35.f;
    rotatedTransform.zIndex = 1;
    rotated.Get<ECS::CollisionComponent>().collisionBox = rotatedTransform.size;
    rotated.Get<ECS::ColorComponent>().color = {110, 155, 175, 255};
    m_ecsManager->AddComponentToEntity<ECS::ShadowComponent>(rotated.GetID());

    auto sprite = m_ecsHelper->CreateStaticObject("ShadowDemoPng");
    auto& spriteTransform = sprite.Get<ECS::TransformComponent>();
    spriteTransform.location = {174.f, 44.f};
    spriteTransform.size = {32.f, 32.f};
    spriteTransform.zIndex = 1;
    sprite.Get<ECS::CollisionComponent>().collisionBox = spriteTransform.size;
    m_textureHelper->SetTextureToEntity(sprite.GetID(), "cat.png");
    m_ecsManager->AddComponentToEntity<ECS::ShadowComponent>(sprite.GetID());

    auto warmLight = m_ecsHelper->CreateLightEntity("ShadowDemoWarmLight");
    auto& warm = warmLight.Get<ECS::LightComponent>();
    warm.location = {65.f, 72.f};
    warm.radius = 5.f; // 160 logical pixels.
    warm.watts = 140.f;
    warm.color = {255, 190, 115, 255};

    auto coolLight = m_ecsHelper->CreateLightEntity("ShadowDemoCoolLight");
    auto& cool = coolLight.Get<ECS::LightComponent>();
    cool.location = {264.f, 54.f};
    cool.radius = 4.f;
    cool.watts = 120.f;
    cool.color = {100, 170, 255, 255};

    GKC_ENGINE_INFO("Shadow demo: ambient 0.15, warm/cool lights, solid and PNG blockers. "
        "Move the player to test its shadow; cat.png must be a transparent texture asset.");
}

void Scene::SpawnPhysicsDebugEntities() {
    SetPixelResolution(320, 180, false);
    SetAmbientLight(1.f);
    m_bShowColliders = true;

    auto player = m_ecsHelper->GetEntityByName("Player");
    auto& transform = player.Get<ECS::TransformComponent>();
    transform.location = {60.f, 75.f};
    transform.size = {12.f, 12.f};
    player.Get<ECS::CollisionComponent>() = ECS::CollisionComponent(transform.size);
    player.Get<ECS::RigidBody>() = ECS::RigidBody();
    player.Get<ECS::SpeedComponent>().maxSpeed = 70.f;
    player.Get<ECS::ColorComponent>().color = {80, 180, 240, 255};

    SpawnPhysicsDebugWalls();
    for (int i = 0; i < 4; ++i) {
        auto box = m_ecsHelper->CreatePhysicsObject("PhysicsDemoBox" + std::to_string(i));
        auto& boxTransform = box.Get<ECS::TransformComponent>();
        boxTransform.location = {110.f + i * 24.f, 75.f};
        boxTransform.size = {16.f, 16.f};
        box.Get<ECS::CollisionComponent>().collisionBox = boxTransform.size;
        box.Get<ECS::RigidBody>().mass = 0.5f;
        box.Get<ECS::ColorComponent>().color = {200, 145, 70, 255};
    }

    auto sensor = m_ecsHelper->CreateStaticObject("PhysicsDemoSensor");
    auto& sensorTransform = sensor.Get<ECS::TransformComponent>();
    sensorTransform.location = {240.f, 60.f};
    sensorTransform.size = {32.f, 50.f};
    auto& collider = sensor.Get<ECS::CollisionComponent>();
    collider.collisionBox = sensorTransform.size;
    collider.isSensor = true;
    sensor.Get<ECS::ColorComponent>().color = {80, 210, 100, 100};
    GKC_ENGINE_INFO("Physics demo: WASD moves the player, brown boxes are pushable, "
        "green is a sensor. Read CollisionSystem::GetEvents() after each fixed tick.");
}

void Scene::SpawnPhysicsDebugWalls() {
    const Render::Vec2 positions[] = {{0.f, 0.f}, {0.f, 172.f}, {0.f, 8.f},
        {312.f, 8.f}, {150.f, 110.f}};
    const Render::Vec2 sizes[] = {{320.f, 8.f}, {320.f, 8.f}, {8.f, 164.f},
        {8.f, 164.f}, {8.f, 40.f}};
    for (int i = 0; i < 5; ++i) {
        auto wall = m_ecsHelper->CreateStaticObject("PhysicsDemoWall" + std::to_string(i));
        auto& transform = wall.Get<ECS::TransformComponent>();
        transform.location = positions[i];
        transform.size = sizes[i];
        wall.Get<ECS::CollisionComponent>().collisionBox = sizes[i];
        wall.Get<ECS::ColorComponent>().color = {110, 115, 125, 255};
    }
}

void Scene::FixedUpdate(float deltaTime, const shared_ptr<Systems::BaseSystem>& movementSystem,
    const shared_ptr<Systems::CameraSystem>& cameraSystem) {
    movementSystem->Update(m_ecsManager->GetEntityList(), deltaTime);
    m_systemList.at("PhysicsSystem")->Update(m_ecsManager->GetEntityList(), deltaTime);
    cameraSystem->Update(m_ecsManager->GetEntityList(), deltaTime,
        m_window->GetViewWidth(), m_window->GetViewHeight());
}

Systems::CollisionSystem& Scene::GetCollisionSystem() {
    return std::static_pointer_cast<Systems::PhysicsSystem>(
        m_systemList.at("PhysicsSystem"))->GetCollisionSystem();
}

void Scene::RenderFrame(float deltaTime, TTF_Font* consoleFont, Console* console,
    const shared_ptr<Systems::CameraSystem>& cameraSystem) {

    // Update textures and animation before either the visible or shadow pass.
    m_rPixelsHelper->DrawAllGroups();
    m_managerWrapper->m_animationManager->UpdateAll(deltaTime);
    m_window->Draw(GKC_GET_RENDERER(m_window));
    Render::Drawer::DrawEntities(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem);

    const bool lightingSucceeded = Render::Drawer::DrawLighting(
        m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem,
        m_lighting, static_cast<int>(m_window->GetViewWidth()),
        static_cast<int>(m_window->GetViewHeight()), m_ambientLight);
    if (!lightingSucceeded && !m_lightingFailed)
        GKC_ENGINE_WARNING("Lighting failed: {}", SDL_GetError());
    m_lightingFailed = !lightingSucceeded;

    if (m_bShowWireframe) {
        Render::Drawer::DrawWireframes(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem);
    }

    if (m_bShowColliders) {
        Render::Drawer::DrawColliders(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem);
    }

    console->Display(GKC_GET_RENDERER(m_window), consoleFont, m_window->GetViewWidth(), m_window->GetViewHeight());

    SDL_RenderPresent(GKC_GET_RENDERER(m_window));
}

void Scene::Save() {
    FileWriter::WriteScene(Config::GetDirectoryOrDefault("scene_path", m_appPath, GKC_SCENE_PATH) / path(m_sceneInfo.scene_name_ + ".gkscene")
        , *this, m_registry);
}

void Scene::Pause() 
{
    SetPaused(!m_bPaused);
}

void Scene::SetPaused(bool paused)
{
    m_bPaused = paused;
    Managers::AudioManager::SetPaused(paused);
    Config::ConfigSetter::SetVariable("scene_paused", paused, "general.cfg");
}

void Scene::SetWireframes(bool visible)
{
    m_bShowWireframe = visible;
    Config::ConfigSetter::SetVariable("show_wireframes", visible, "general.cfg");
}

void Scene::SetColliders(bool visible)
{
    m_bShowColliders = visible;
    Config::ConfigSetter::SetVariable("show_colliders", visible, "general.cfg");
}

void Scene::ToggleWireframes()
{
    SetWireframes(!m_bShowWireframe);
}

void Scene::ToggleColliders()
{
    SetColliders(!m_bShowColliders);
}

void Scene::OnEvent(Events::GKC_Event& event) {
    GKC_ASSERT(!m_systemList.empty(), "system manager is empty!");

    if (auto* console = Core::App::GetConsole()) {
        console->OnEvent(event);
        if (event.m_handled)
            return;
    }
    
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
    cameraComponent.zoom = Config::GetValueOrDefault<float>("general.cfg", "camera_zoom", cameraComponent.zoom,
        [](float value) { return value > 0.f; });
    cameraComponent.smoothing = Config::GetValueOrDefault<float>("general.cfg", "camera_smoothing", cameraComponent.smoothing,
        [](float value) { return value >= 0.f; });
    cameraComponent.isActive = true;
    m_ecsHelper->CreateCameraEntity(name);
    m_ecsHelper->ModifyEntity(name, cameraComponent);
}

void Scene::Close() {
    m_isRunning = false;
}

void Scene::SetPixelResolution(int width, int height, bool integerScale)
{
    m_window->SetPixelResolution(width, height, integerScale);
}

void Scene::ToggleFullscreen()
{
    m_window->ToggleFullscreen();
}

void Scene::SetFullscreen(bool enabled) {
    m_window->SetFullScreen(enabled);
}

void Scene::SetAmbientLight(float value)
{
    if (std::isfinite(value)) m_ambientLight = std::clamp(value, 0.f, 1.f);
}

float Scene::GetAmbientLight() const
{
    return m_ambientLight;
}
