#include <core/gkc_app.h>
#include <core/gkc_logger.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_debugger.h>
#include <core/gkc_scene.h>
#include "core/gkc_exception.h"

using namespace Galaktic::Core;

void App::ScreenStartup() {
    m_deviceInfo.os_ = GKC_OS;
    m_deviceInfo.arch_ = GKC_ARCH;
    m_deviceInfo.width_ = 800;
    m_deviceInfo.height_ = 600;

    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_id);

    GKC_ASSERT(display_id >= 1, "Failed to get the primary display!");
    GKC_ASSERT(mode != nullptr, "Failed to get the display mode!");

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Display ID: {0}", display_id);
        GKC_ENGINE_INFO("Current Display Mode: {0}x{1} @ {2}Hz", mode->w, mode->h, mode->refresh_rate);
    #endif

    m_deviceInfo.width_ = mode->w;
    m_deviceInfo.height_ = mode->h;
}

void App::LoadSpecificScene(const string& scene_name) {
    if (!m_sceneList.contains(scene_name)) {
        GKC_THROW_EXCEPTION(Debug::FilesystemException, "Scene not found!");
    }
    m_sceneList.find(scene_name)->second.Run();
}

App::App(const path& project_path, const string &title)
    : m_appName(title) {

    Debug::Logger::PrintEngineInformation();
    Debug::StartLibraries();
    Filesystem::CreateFolder(title);
    Filesystem::CreateAppDirectoryStructure(project_path);
    ScreenStartup();

    // Create a new default Scene
    m_sceneList.emplace("Main", Scene("Main", m_deviceInfo, project_path));
}

