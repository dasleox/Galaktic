#include <core/systems/gkc_ui_system.h>
#include "core/gkc_debugger.h"
#include "core/events/gkc_dispatcher.h"
#include "render/gkc_window.h"

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;

void UISystem::OnEvent(Events::GKC_Event &e) {
    Events::GKC_EventDispatcher dispatcher(e);
    dispatcher.DispatchEvent<Events::KeyPressedEvent>(
        [this](Events::KeyPressedEvent &e) {
            if (GKC_SDLK_TO_ENUM(e) == Key::F3) {
                if (Debug::Console::GetIsActive() == true)
                    Debug::Console::GetIsActive() = false;
                else
                    Debug::Console::GetIsActive() = true;
            }
            return false;
        });

    dispatcher.DispatchEvent<Events::KeyPressedEvent>(
        [this](Events::KeyPressedEvent &e) {
            if (GKC_SDLK_TO_ENUM(e) == Key::F11)
                Render::Window::SetFullScreen();
            return false;
        });
}
