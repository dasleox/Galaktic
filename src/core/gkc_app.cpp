#include <core/gkc_app.h>
#include <core/gkc_logger.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_debugger.h>
#include <core/gkc_scene.h>
#include <core/managers/gkc_scene_man.h>
#include <core/managers/gkc_audio_man.h>
#include <core/managers/gkc_texture_man.h>
#include <core/managers/gkc_script_man.h>
#include <core/gkc_console.h>
#include <core/managers/gkc_animation_man.h>
#include <core/managers/gkc_config_manager.h>
#include <core/managers/gkc_font_man.h>
#include <script/gkc_library.h>
#include <config/gkc_config.h>
#include <config/gkc_config_file.h>
#include <config/gkc_config_values.h>
#include <core/gkc_console.h>

using std::make_unique;
using std::filesystem::path;
using std::string;
using std::unique_ptr;

using namespace Galaktic::Core;

unique_ptr<Console> App::m_console = nullptr;

void App::ScreenStartup() {
    m_deviceInfo.os_ = GKC_OS;
    m_deviceInfo.arch_ = GKC_ARCH;
    m_deviceInfo.width_ = 800;
    m_deviceInfo.height_ = 600;

    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_id);

    if(display_id < 1) {
        Debug::Logger::LogErrorWithType(ErrorType::LibraryError, "Failed to get the primary display!");
    } 
    if (mode == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::LibraryError, "Failed to get the display mode!");
        return;
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

    Debug::StartLibraries();

    m_console = make_unique<Console>(Debug::Logger::GetConsoleMutex());

    Filesystem::CreateFolder(project_path / title);
    Filesystem::CreateAppDirectoryStructure(project_path / title);
    const auto projectRoot = project_path / title;
    Managers::ConfigManager::SetFolder(projectRoot / GKC_CONFIG_PATH);
    m_bindConfigurator = make_unique<Config::BindConfigurator>(projectRoot / GKC_CONFIG_PATH / "bind.cfg");
    ScreenStartup();

    m_managersWrapper = make_unique<ManagersWrapper>();
    GKC_RELEASE_ASSERT(m_managersWrapper != nullptr, "Failed to create the managers wrapper!");
    m_managersWrapper->m_audioManager = new Managers::AudioManager(
        Config::GetDirectoryOrDefault("sound_path", projectRoot, GKC_SOUND_PATH).string());
    m_managersWrapper->m_textureManager = new Managers::TextureManager(
        Config::GetDirectoryOrDefault("texture_path", projectRoot, GKC_TEXTURE_PATH).string());
    m_managersWrapper->m_animationManager = new Managers::AnimationManager(
        Config::GetDirectoryOrDefault("animation_path", projectRoot, GKC_ANIMATION_PATH).string());
    m_managersWrapper->m_fontManager = new Managers::FontManager(
        Config::GetDirectoryOrDefault("font_path", projectRoot, "assets/fonts"));
    const auto scriptRoot = Config::GetDirectoryOrDefault("script_path", projectRoot, GKC_SCRIPT_PATH);
    
    GKC_RELEASE_ASSERT(Script::LuaGalaktic::Initialize(), "Failed to initialize Lua!");
    Script::LuaGalaktic::SetupModules(scriptRoot);
    Script::LuaGalaktic::BindGalaktic();
    m_managersWrapper->m_scriptManager = new Managers::ScriptManager(scriptRoot.string(), Script::LuaGalaktic::GetLuaState());

    // The scene manager is created last: it owns scene creation/loading from
    // here on, and every manager it may hand to a scene already exists above.
    m_sceneManager = new Managers::SceneManager(project_path / title, m_managersWrapper.get(), m_deviceInfo);
    m_console->SetSceneManager(m_sceneManager);

}
