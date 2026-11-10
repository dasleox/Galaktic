#include <core/systems/gkc_mouse_system.h>
#include <core/events/gkc_event.h>
#include "core/gkc_logger.h"
#include "core/events/gkc_dispatcher.h"

using std::string;
using std::unordered_map;

using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core;

MouseSystem* Mouse::m_mouseSystem = nullptr;
unordered_map<string, MouseClick> Mouse::m_clickMap = {
    {"LeftClick", MouseClick::LeftClick},
    {"RightClick", MouseClick::RightClick},
    {"MiddleClick", MouseClick::MiddleClick}
};

void MouseSystem::OnEvent(Events::GKC_Event &e) {
    Events::GKC_EventDispatcher dispatcher(e);

    dispatcher.DispatchEvent<Events::MouseClickEvent>(
        [this](Events::MouseClickEvent& e) {
            m_mouseState[GKC_SDLMOUSE_TO_ENUM(e)] = e.IsPressed();
            return false;
        }
    );
}


Mouse::Mouse(MouseSystem* mouseSystem) {
    m_mouseSystem = mouseSystem;
}

bool Mouse::IsMouseDown(MouseClick click) {
    return m_mouseSystem != nullptr && m_mouseSystem->IsMouseClick(click);
}
bool Mouse::IsMouseDownLua(Uint32 click) {
    auto validClick = static_cast<Uint32>(click);
    return IsMouseDown(static_cast<MouseClick>(validClick));
}

MouseClick Mouse::StringToMouseClick(const string& type) {
    auto it = m_clickMap.find(type);
    if(it != m_clickMap.end())
        return it->second;
    return MouseClick::LeftClick;
}
