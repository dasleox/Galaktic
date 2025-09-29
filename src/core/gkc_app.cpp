#include <core/gkc_app.h>
#include <core/gkc_main.h>
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include <iostream>


void Galaktic::Core::App::Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {

    }
}