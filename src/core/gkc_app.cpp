#include <core/gkc_app.h>
#include <core/gkc_logger.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_debugger.h>
#include <core/gkc_scene.h>
#include "core/gkc_exception.h"
#include <core/managers/gkc_scene_man.h>
#include <core/managers/gkc_audio_man.h>
#include <core/managers/gkc_texture_man.h>
#include <core/managers/gkc_script_man.h>
#include <core/managers/gkc_animation_man.h>
#include <script/gkc_library.h>
#include <config/gkc_config.h>

using namespace Galaktic::Core;

void App::ScreenStartup() {
    m_deviceInfo.os_ = GKC_OS;
    m_deviceInfo.arch_ = GKC_ARCH;
    m_deviceInfo.width_ = 800;
    m_deviceInfo.height_ = 600;

    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_id);

    if(display_id < 1) {
        GKC_ENGINE_ERROR("Failed to get the primary display!");
    } 
    if (mode == nullptr) {
        GKC_ENGINE_ERROR("Failed to get the display mode!");
    }

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Display ID: {0}", display_id);
        GKC_ENGINE_INFO("Current Display Mode: {0}x{1} @ {2}Hz", mode->w, mode->h, mode->refresh_rate);
    #endif

    m_deviceInfo.width_ = mode->w;
    m_deviceInfo.height_ = mode->h;

}

App::App(const path& project_path, const string &title)
    : m_appName(title) {

    Debug::Logger::PrintEngineInformation();
    Debug::StartLibraries();
    Filesystem::CreateFolder(title);
    Filesystem::CreateAppDirectoryStructure(project_path / title);
    ScreenStartup();
    
    GKC_RELEASE_ASSERT(Script::LuaGalaktic::Initialize(), "Failed to initialize Lua!");
    Script::LuaGalaktic::BindGalaktic();

    m_managersWrapper = make_unique<ManagersWrapper>();
    GKC_RELEASE_ASSERT(m_managersWrapper != nullptr, "CRITICAL ERROR CREATING MANAGER WRAPPER!");
    m_managersWrapper->m_audioManager = new Managers::AudioManager(path(project_path / title /GKC_SOUND_PATH).string());
    m_managersWrapper->m_textureManager = new Managers::TextureManager(path(project_path / title / GKC_TEXTURE_PATH).string());
    m_managersWrapper->m_scriptManager = new Managers::ScriptManager(path(project_path / title / GKC_SCRIPT_PATH).string(), Script::LuaGalaktic::GetLuaState());
    m_managersWrapper->m_animationManager = new Managers::AnimationManager(path(project_path / title / GKC_ANIMATION_PATH).string());

    // Execute scripts to init managers

    m_sceneManager = new Managers::SceneManager(project_path / title, m_managersWrapper.get(), m_deviceInfo);
}

