#include <core/systems/gkc_window_system.h>

#include "core/gkc_exception.h"
#include "core/events/gkc_dispatcher.h"

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;

void WindowSystem::OnEvent(Events::GKC_Event &e) {
    Events::GKC_EventDispatcher dispatcher(e);

    dispatcher.DispatchEvent<Events::WindowCloseEvent>(
        [this](Events::WindowCloseEvent &e) {
            exit(0);
            return false;
        });

    dispatcher.DispatchEvent<Events::WindowResizeEvent>(
        [this](Events::WindowResizeEvent &e) {
            SDL_Window* window = SDL_GetWindowFromID(e.GetWindowID());
            SDL_Renderer* renderer = SDL_GetRenderer(window);
            if (window == nullptr || renderer == nullptr) {
                GKC_THROW_EXCEPTION(Debug::RenderException, "window to resize doesn't exist!");
            }

            // This is unsafe as fuck if a clown decides to put the window size more than the
            // 32-bit signed integer limit
            // @todo Add a function to resize the lightness buffer
            SDL_SetWindowSize(window, e.GetWidth(), e.GetHeight());
            SDL_Rect dimensions = {0, 0, (int) e.GetWidth(), (int) e.GetHeight()};
            SDL_SetRenderViewport(renderer, &dimensions);
            return false;
        });
}
