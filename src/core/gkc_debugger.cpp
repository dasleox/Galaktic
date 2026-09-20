#include <core/gkc_debugger.h>
#include "core/gkc_logger.h"

using namespace Galaktic::Debug;

void Galaktic::Debug::StartLibraries() {
    Logger::Init();

    GKC_RELEASE_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) && TTF_Init() && MIX_Init(), "SDL could not initialize!");
    GKC_ENGINE_INFO("Initialized SDL successfully.");
}

Galaktic::Core::DeviceInformation Galaktic::Debug::GetDeviceInformation() {
    Core::DeviceInformation info;
    info.os_ = GKC_OS;
    info.arch_ = GKC_ARCH;
    info.width_ = 800;
    info.height_ = 600;

    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_id);

    GKC_ASSERT(display_id >= 1, "Failed to get the primary display!");
    GKC_ASSERT(mode != nullptr, "Failed to get the display mode!");

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Display ID: {0}", display_id);
        GKC_ENGINE_INFO("Current Display Mode: {0}x{1} @ {2}Hz", mode->w, mode->h, mode->refresh_rate);
    #endif

    info.width_ = mode->w;
    info.height_ = mode->h;

    return info;
}
