#include <core/gkc_app.h>
#include <core/gkc_main.h>
#include <core/gkc_error.h>
#include <core/gkc_logger.h>

#include <graphics/gkc_renderer.h>
#include <graphics/gkc_window.h>

#include <pch.hpp>

using namespace Galaktic::Core;

Galaktic::Core::App::App(string title, AppType type) {
    // App Info Setup
    app_info_.app_name_ = title;
    app_info_.app_type_ = type;

    device_info_.os_ = GKC_OS;
    device_info_.arch_ = GKC_ARCH;
    device_info_.width_ = 800;
    device_info_.height_ = 600;
}

void Galaktic::Core::App::Init() {
    InitLibraries();
    Debug::Logger::Init();
    Debug::PrintEngineInformation();
    
    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_id);

    if(display_id == 0 || mode == nullptr) {
        GKC_ENGINE_ERROR("Failed to get display information. Using default resolution 800x600.");
    } else {
        GKC_ENGINE_INFO("Display ID: {0}", display_id);
        GKC_ENGINE_INFO("Current Display Mode: {0}x{1} @ {2}Hz", mode->w, mode->h, mode->refresh_rate);
    }

    renderer_ = new Renderer::Renderer(device_info_, app_info_);
    
    GKC_CLIENT_INFO("Application initialized: {0}", app_info_.app_name_);
}

void Galaktic::Core::App::Run() {
    Init();
    GKC_CLIENT_INFO("Running application: {0}", app_info_.app_name_);
    Loop();
}

void Galaktic::Core::App::Loop() {
    bool is_running = true;
    while (is_running) {
        while (SDL_PollEvent(&event_)) {
            if(SDL_EVENT_QUIT == event_.type) {
                Free();
                is_running = false;
            }
        }

        // Rendering
        renderer_->window_->DrawWindow(GKC_GET_RENDERER(renderer_));
        SDL_Delay(16);
    }

    Free();
}

void Galaktic::Core::App::Free() {
    delete renderer_;
    SDL_Quit();
    GKC_CLIENT_INFO("Application terminated: {0}", app_info_.app_name_);
}