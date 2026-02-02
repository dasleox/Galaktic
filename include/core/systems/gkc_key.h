/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

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
#include "gkc_system.h"

namespace Galaktic::Core::Systems {
    /**
     * @enum Key
     * @brief Enum class representing keyboard keys
     */
    enum class Key : uint32_t {
        // =============================
        // Unknown / Control
        // =============================
        Unknown = SDLK_UNKNOWN,

        Return    = SDLK_RETURN,
        Escape    = SDLK_ESCAPE,
        Backspace = SDLK_BACKSPACE,
        Tab       = SDLK_TAB,
        Space     = SDLK_SPACE,

        // =============================
        // Numbers (Top Row)
        // =============================
        Key0 = SDLK_0,
        Key1 = SDLK_1,
        Key2 = SDLK_2,
        Key3 = SDLK_3,
        Key4 = SDLK_4,
        Key5 = SDLK_5,
        Key6 = SDLK_6,
        Key7 = SDLK_7,
        Key8 = SDLK_8,
        Key9 = SDLK_9,

        // =============================
        // Letters
        // =============================
        A = SDLK_A,
        B = SDLK_B,
        C = SDLK_C,
        D = SDLK_D,
        E = SDLK_E,
        F = SDLK_F,
        G = SDLK_G,
        H = SDLK_H,
        I = SDLK_I,
        J = SDLK_J,
        K = SDLK_K,
        L = SDLK_L,
        M = SDLK_M,
        N = SDLK_N,
        O = SDLK_O,
        P = SDLK_P,
        Q = SDLK_Q,
        R = SDLK_R,
        S = SDLK_S,
        T = SDLK_T,
        U = SDLK_U,
        V = SDLK_V,
        W = SDLK_W,
        X = SDLK_X,
        Y = SDLK_Y,
        Z = SDLK_Z,

        // =============================
        // Function Keys
        // =============================
        F1  = SDLK_F1,
        F2  = SDLK_F2,
        F3  = SDLK_F3,
        F4  = SDLK_F4,
        F5  = SDLK_F5,
        F6  = SDLK_F6,
        F7  = SDLK_F7,
        F8  = SDLK_F8,
        F9  = SDLK_F9,
        F10 = SDLK_F10,
        F11 = SDLK_F11,
        F12 = SDLK_F12,

        // =============================
        // Arrows
        // =============================
        Left  = SDLK_LEFT,
        Right = SDLK_RIGHT,
        Up    = SDLK_UP,
        Down  = SDLK_DOWN,

        // =============================
        // Modifiers
        // =============================
        LeftShift   = SDLK_LSHIFT,
        RightShift  = SDLK_RSHIFT,
        LeftCtrl    = SDLK_LCTRL,
        RightCtrl   = SDLK_RCTRL,
        LeftAlt     = SDLK_LALT,
        RightAlt    = SDLK_RALT,
        LeftMeta    = SDLK_LGUI,
        RightMeta   = SDLK_RGUI,

        // =============================
        // Navigation
        // =============================
        Insert   = SDLK_INSERT,
        Delete   = SDLK_DELETE,
        Home     = SDLK_HOME,
        End      = SDLK_END,
        PageUp   = SDLK_PAGEUP,
        PageDown = SDLK_PAGEDOWN,

        // =============================
        // Keypad
        // =============================
        KeyPad0 = SDLK_KP_0,
        KeyPad1 = SDLK_KP_1,
        KeyPad2 = SDLK_KP_2,
        KeyPad3 = SDLK_KP_3,
        KeyPad4 = SDLK_KP_4,
        KeyPad5 = SDLK_KP_5,
        KeyPad6 = SDLK_KP_6,
        KeyPad7 = SDLK_KP_7,
        KeyPad8 = SDLK_KP_8,
        KeyPad9 = SDLK_KP_9,

        KeyPadPlus     = SDLK_KP_PLUS,
        KeyPadMinus    = SDLK_KP_MINUS,
        KeyPadMultiply = SDLK_KP_MULTIPLY,
        KeyPadDivide   = SDLK_KP_DIVIDE,
        KeyPadEnter    = SDLK_KP_ENTER, // 0x40000058
        KeyPadDecimal  = SDLK_KP_DECIMAL
    };

    /**
     * @brief Casts a SDL_Keycode to a Key enumerator
     * @param key The SDL_Keycode
     * @return A Key enumerator
     */
    inline Key SDL_KeyToKeyEnum(SDL_Keycode key) {
        return static_cast<Key>(key);
    }
}

namespace Galaktic::Core::Systems {
    /**
     * @class KeySystem
     * @brief Maps the keys depending on the type of key event
     * 
     * Handles key press and release events to maintain the current state of each key.
     * Provides functionality for other systems like the \c MovementSystem to check if a 
     * specific key is currently pressed to execute actions accordingly.
     */
    class KeySystem final : public BaseSystem {
        public:
            /**
             * @brief Map a key depending on the type of the key event\n
             *        map true if a key was pressed, false otherwise
             * @param e GKC_Event
             */
            void OnEvent(Events::GKC_Event &e) override;

            /**
             * @brief Checks if a key exists on the map and returns the value mapped
             * @param key Key enumerator
             * @return true if the key is down
             */
            bool IsKeyDown(Key key) {
                return m_keyState[key];
            }
        private:
            unordered_map<Key, bool> m_keyState;
    };
}

/**
 * @brief Transforms a SDL_Keycode from an GKC_Event to a Key enum
 * @param e GKC_Event
 */
#define GKC_SDLK_TO_ENUM(e) SDL_KeyToKeyEnum(e.GetKeyCode())