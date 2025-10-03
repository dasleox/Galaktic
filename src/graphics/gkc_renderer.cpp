#include <graphics/gkc_renderer.h>
#include <graphics/gkc_window.h>
#include <core/gkc_main.h>
#include <core/gkc_logger.h>
#include <core/gkc_app.h>
#include <pch.hpp>

using namespace Galaktic::Core;

Galaktic::Renderer::Renderer::Renderer (DeviceInformation& device_info, AppInformation& app_info) {
    string title = app_info.app_name_;
    Uint16 width = device_info.width_;
    Uint16 height = device_info.height_;
    
    window_ = new Window(title, width, height);

    renderer_ = SDL_CreateRenderer(GKC_GET_WINDOW(window_), NULL);

    GKC_ENGINE_INFO("Renderer created.");
}