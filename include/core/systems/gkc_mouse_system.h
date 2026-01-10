/*
  Galaktic Engine
  Copyright (C) 2025 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#include <pch.hpp>
#include <core/systems/gkc_system.h>

// ###################################
// ### Enums
// ###################################
namespace Galaktic::Core::Systems {
    enum class MouseClick : uint32_t {
        LeftClick = SDL_BUTTON_LEFT,
        RightClick = SDL_BUTTON_RIGHT,
        MiddleClick = SDL_BUTTON_MIDDLE
    };
}

namespace Galaktic::Core::Systems {
    /**
     * @class MouseSystem
     * @brief Maps mouse buttons if they were pressed or not
     */
    class MouseSystem final : public BaseSystem {
        public:
            /**
             * @brief Map a mouse button depending on the type of the mouse event\n
             *        map true if a key was pressed, false otherwise
             * @param e GKC_Event
             */
            void OnEvent(Events::GKC_Event &e) override;

            /**
             * @brief Checks if a mouse button exists on the map and returns the value mapped
             * @param type MouseClick
             * @return true if the button is down
             */
            bool IsMouseClick(MouseClick type) {
                return m_mouseState[type];
            }
        private:
            unordered_map<MouseClick, bool> m_mouseState;
    };

    /**
       * @brief Casts a SDL_MouseButtonFlags to a MouseClick enumerator
       * @param button The SDL_MouseButtonFlags
       * @return A MouseClick enumerator
     */
    inline MouseClick SDL_MouseButtonToClickEnum(SDL_MouseButtonFlags button) {
        return static_cast<MouseClick>(button);
    }
}

/**
 * @brief Transforms a SDL_MouseButtonFlags from an GKC_Event to a MouseClick enum
 * @param e GKC_Event
 */
#define GKC_SDLMOUSE_TO_ENUM(e) SDL_MouseButtonToClickEnum(e.GetButton())