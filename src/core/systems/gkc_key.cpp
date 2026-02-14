#include <core/systems/gkc_key.h>
#include <core/systems/gkc_system.h>
#include "core/events/gkc_dispatcher.h"

using namespace Galaktic::Core::Systems;

KeySystem* Keyboard::m_keySystem = nullptr;

void KeySystem::OnEvent(Events::GKC_Event &e) {
    Events::GKC_EventDispatcher dispatcher(e);;
    dispatcher.DispatchEvent<Events::KeyPressedEvent>(
        [this](Events::KeyPressedEvent &e) {
            m_keyState[GKC_SDLK_TO_ENUM(e)] = true;
            return false;
        }
    );
    
    dispatcher.DispatchEvent<Events::KeyHoldEvent>(
        [this](Events::KeyHoldEvent &e) {
            m_keyState[GKC_SDLK_TO_ENUM(e)] = false;
            return false;
        });
}

bool Galaktic::Core::Systems::Keyboard::IsKeyDown(Key key) {
    return m_keySystem->IsKeyDown(key);
}