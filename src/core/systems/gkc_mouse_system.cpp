#include <core/systems/gkc_mouse_system.h>
#include <core/events/gkc_event.h>
#include "core/gkc_logger.h"
#include "core/events/gkc_dispatcher.h"

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;


void MouseSystem::OnEvent(Events::GKC_Event &e) {
    Events::GKC_EventDispatcher dispatcher(e);

    dispatcher.DispatchEvent<Events::MouseClickEvent>(
        [this](Events::MouseClickEvent& e) {
            if (GKC_SDLMOUSE_TO_ENUM(e) == MouseClick::LeftClick) {
                m_mouseState[GKC_SDLMOUSE_TO_ENUM(e)] = true;
            }
            return false;
        }
    );

    dispatcher.DispatchEvent<Events::MouseClickEvent>(
        [this](Events::MouseClickEvent& e) {
            if (GKC_SDLMOUSE_TO_ENUM(e) == MouseClick::RightClick) {
                m_mouseState[GKC_SDLMOUSE_TO_ENUM(e)] = true;
            }
            return false;
        }
    );

    dispatcher.DispatchEvent<Events::MouseClickEvent>(
        [this](Events::MouseClickEvent& e) {
            if (GKC_SDLMOUSE_TO_ENUM(e) == MouseClick::MiddleClick) {
                m_mouseState[GKC_SDLMOUSE_TO_ENUM(e)] = true;
            }
            return false;
        }
    );
}
