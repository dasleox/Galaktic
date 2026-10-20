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
#include "core/managers/gkc_font_man.h"

#include <numeric>
#include <random>

using namespace Galaktic::Core;
using namespace Galaktic::Filesystem;

namespace {
    /**
     * @brief Builds a pixel mask with a given fraction of its pixels selected at random
     * @param width Width of the mask to build
     * @param height Height of the mask to build
     * @param selectedRatio Fraction of pixels (0-1) that should be selected
     * @return A width x height mask, addressed as mask[y][x]
     */
    Galaktic::Render::PixelMapping_List BuildRandomPixelMask(size_t width, size_t height, float selectedRatio) {
        Galaktic::Render::PixelMapping_List mask(height, std::vector<bool>(width, false));

        const size_t total = width * height;
        const size_t selectedCount = static_cast<size_t>(static_cast<float>(total) * selectedRatio);

        std::vector<size_t> indices(total);
        std::iota(indices.begin(), indices.end(), 0);

        std::mt19937 rng(std::random_device{}());
        std::shuffle(indices.begin(), indices.end(), rng);

        for (size_t i = 0; i < selectedCount; ++i) {
            size_t row = indices[i] / width;
            size_t col = indices[i] % width;
            mask[row][col] = true;
        }

        return mask;
    }
}

Scene::Scene(const string& name, ManagersWrapper* wrapper, const DeviceInformation& device_information, const path& path)
    : m_sceneInfo({name, sizeof(Scene)}), m_managerWrapper(wrapper) {

        
    GKC_ASSERT(!device_information.IsCorrupted(), "information from app is corrupted!");
    if (Managers::WindowManager::GetWindowList().empty()) {
        m_window = make_shared<Render::Window>(name, device_information.width_,
            device_information.height_, Render::Window_Type::Resizable);
        Managers::WindowManager::RegisterWindow(m_window);
    } else {
        m_window = Managers::WindowManager::GetWindow(1);
        m_window->SetTitle(name);
    }

    m_managerWrapper->m_textureManager->CreateMissingTexture(GKC_GET_RENDERER(m_window));

    m_appPath = path;

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
    auto physicsSystem = make_shared<Systems::PhysicsSystem>();
    auto uiSystem = make_shared<Systems::UISystem>(*keySystem);
    auto windowSystem = make_shared<Systems::WindowSystem>();
    auto cameraSystem = make_shared<Systems::CameraSystem>(camera);
    auto entityEventSystem = make_shared<Systems::ECS_EventSystem>(*m_ecsManager);

    // @todo Entity ID 2 is the player by convention (Camera is created first,
    // Player second - see ECS_Helper::CreatePlayer). Support following an
    // explicitly chosen entity instead of relying on creation order.
    cameraSystem->SetFollowEntity(2);

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
    auto sceneScriptFolderPath = m_appPath / GKC_SCRIPT_PATH / "local" / name;
    if (!Filesystem::CheckDirectory(sceneScriptFolderPath)) {
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

    //@todo Add for fonts, audio and scripts inside their own functions

    //@bug Fix a bug where RPixels are not reloaded when their texture is reloaded, causing them to be invisible until the scene is restarted
}

void Scene::Run()  {
    LoadRunAssets();

    // Route SDL events polled by the window into the scene's event pipeline
    m_window->SetCallback(
        [this](Events::GKC_Event& event) {
            OnEvent(event);
        }
    );

    auto console = Core::App::GetConsole();
    console->GetSceneContext(this); 


    auto fontPath = path(m_appPath / "assets" / "fonts" / "console_font.ttf");
    TTF_Font* consoleFont = TTF_OpenFont(fontPath.string().c_str(), 13.f);
    if (consoleFont == nullptr)
    {
        GKC_ENGINE_WARNING("Failed to open console font '{}', falling back to the engine's default font.", fontPath.string());
        consoleFont = Managers::FontManager::GetDefaultFont();
    }
    if (consoleFont == nullptr)
    {
        GKC_ENGINE_ERROR("No usable font is available; the console will render without text.");
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

    SpawnDebugDemoEntities();

    while (m_isRunning) 
    {
        Clock::Update();
        double deltaTime = std::min(Clock::GetDeltaTime(), 0.25);
        accumulator += deltaTime;

        ReloadAssets(deltaTime);

        m_window->PollEvents();
        if (m_window->ShouldClose()) {
            Save();
            Close();
        }

        // @todo PhysicsSystem needs to be reworked to support the newer entity
        // types before it runs again here; MovementSystem/CameraSystem are
        // enough to drive the current sandbox scene.
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
    auto fontPath = m_appPath / "assets" / "fonts" / "minecraft.ttf";
    TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), 12.f);
    if (font == nullptr) {
        GKC_ENGINE_ERROR("Failed to open the debug console font at '{}'", fontPath.string());
    }
    return font;
}

void Scene::SpawnDebugDemoEntities() {
    auto& player = m_ecsHelper->GetEntityByName("Player");

    // @bug Newly created entities briefly report a garbage TransformComponent
    // position until the next OnEvent pass corrects it. Tracked separately
    // from this cleanup pass.
    ECS::Entity testObject = m_ecsHelper->CreateStaticObject("myObject");

    m_textureHelper->SetTextureToEntity(player.GetID(), "cat.png");
    m_textureHelper->SetTextureToEntity(testObject.GetID(), "cat.png");

    SDL_Color mainColor = {123, 42, 11, 255};
    Render::PixelColoredGroupInfo group("group", mainColor, player.GetID(), *m_ecsManager);

    group.AddEntityToGroup(testObject.GetID());
    group.SetSelectedPixels(BuildRandomPixelMask(group.GetWidth(), group.GetHeight(), 0.2f));
    group.ApplyColorToEntities();
}

void Scene::FixedUpdate(float deltaTime, const shared_ptr<Systems::BaseSystem>& movementSystem,
    const shared_ptr<Systems::CameraSystem>& cameraSystem) {
    movementSystem->Update(m_ecsManager->GetEntityList(), deltaTime);
    cameraSystem->Update(m_ecsManager->GetEntityList(), deltaTime,
        m_window->GetWidth(), m_window->GetHeight());
}

void Scene::RenderFrame(float deltaTime, TTF_Font* consoleFont, Console* console,
    const shared_ptr<Systems::CameraSystem>& cameraSystem) {
    m_window->Draw(GKC_GET_RENDERER(m_window));
    Render::Drawer::DrawEntities(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem);

    m_rPixelsHelper->DrawAllGroups();
    m_managerWrapper->m_animationManager->UpdateAll(deltaTime);

    if (m_bShowWireframe) {
        Render::Drawer::DrawWireframes(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem);
    }

    if (m_bShowColliders) {
        Render::Drawer::DrawColliders(m_ecsManager->GetEntityList(), GKC_GET_RENDERER(m_window), *cameraSystem);
    }

    console->Display(GKC_GET_RENDERER(m_window), consoleFont, m_window->GetWidth(), m_window->GetHeight());

    SDL_RenderPresent(GKC_GET_RENDERER(m_window));
}

void Scene::Save() {
    FileWriter::WriteScene(m_appPath / "scenes" / path(m_sceneInfo.scene_name_ + ".gkscene")
        , *this, m_registry);
}

void Scene::Pause() 
{
    m_bPaused = !m_bPaused;
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