#include <core/systems/gkc_window_system.h>

#include "core/events/gkc_dispatcher.h"
#include "core/managers/gkc_window_man.h"
#include "render/gkc_window.h"
#include "core/gkc_error.h"
#include "core/gkc_logger.h"

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;

void WindowSystem::OnEvent(Events::GKC_Event &e) {
    Events::GKC_EventDispatcher dispatcher(e);

    dispatcher.DispatchEvent<Events::WindowCloseEvent>(
        [this](Events::WindowCloseEvent &e) {
            auto window = Managers::WindowManager::GetWindow(e.GetWindowID());
            window->RequestClose();
            return false;
        });

    dispatcher.DispatchEvent<Events::WindowResizeEvent>(
        [this](Events::WindowResizeEvent &e) {
            auto window = Managers::WindowManager::GetWindow(e.GetWindowID()).get();

            SDL_Window* sdlWindow  = SDL_GetWindowFromID(window->GetSDLWindowID());
            SDL_Renderer* renderer = SDL_GetRenderer(sdlWindow);
            if (window == nullptr || renderer == nullptr || sdlWindow == nullptr) {
                Debug::Logger::LogFatalWithType(ErrorType::WindowError, "window to resize doesn't exist!");
                return false;
            }

            SDL_SetWindowSize(sdlWindow, e.GetWidth(), e.GetHeight());
            SDL_Rect dimensions = {0, 0, (int) e.GetWidth(), (int) e.GetHeight()};
            SDL_SetRenderViewport(renderer, &dimensions);
            return false;
        });
}
