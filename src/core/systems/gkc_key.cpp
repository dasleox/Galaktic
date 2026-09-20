#include <core/systems/gkc_key.h>
#include <core/systems/gkc_system.h>
#include "core/events/gkc_dispatcher.h"
#include <core/gkc_logger.h>
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

bool Galaktic::Core::Systems::Keyboard::IsKeyDownLua(lua_Integer key) {
    if (m_keySystem == nullptr) {
        GKC_ENGINE_ERROR("m_keySystem is null!");
        return false;
    }
    
    Uint32 validKey = static_cast<Uint32>(key);
    bool result = m_keySystem->IsKeyDown(static_cast<Key>(validKey));
    return result;
}