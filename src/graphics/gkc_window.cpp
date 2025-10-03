#include <graphics/gkc_window.h>
#include <core/gkc_logger.h>
#include <core/gkc_main.h>
#include <pch.hpp>

using std::string;

Galaktic::Renderer::Window::Window(string& title, Uint16 width, Uint16 height) {
    title_ = title;
    width_ = width;
    height_ = height;

    window_ = SDL_CreateWindow(title_.c_str(), width, height, SDL_WINDOW_RESIZABLE);
    GKC_ENGINE_INFO("Window created: {0}", title_);
}

void Galaktic::Renderer::Window::DrawWindow(SDL_Renderer* renderer)
{
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, GKC_WINDOW_DEF_COLOR);
    SDL_RenderPresent(renderer);
}
