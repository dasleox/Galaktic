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

/**
 * @brief This macro overrides functions from the base class \c GKC_Event
 *        with the given arguments
 *
 * @param type The type of the event
 * @param name The name of the event
 * @param category The category of the event
 * @note The type \b SHOULD exist in \c GKC_EventType enum
 */
#define GKC_WRITE_EVENT_FUNCS(type, name, category) \
    static GKC_EventType GetStaticType() {          \
        return GKC_EventType::type;                 \
    }                                               \
    GKC_EventType GetEventType() const override {   \
        return GetStaticType();                     \
    }                                               \
    const char* GetName() const override {          \
        return name;                                \
    }                                               \
    int GetCategoryFlags() const override {         \
        return GKC_EventCategory::category;         \
    }

namespace Galaktic::Core::Events {
    /**
     * @enum GKC_EventType
     * @brief Event types enumerator
     *
     * This enumerator identifies an event type, in order for event
     * classes an enumerator has to be added here to work.
     */
    enum class GKC_EventType {
        None = 0,
        KeyPressed,
        KeyHold,
        MouseClick,
        WindowResize,
        WindowClose,
        WindowMinimized
    };

    /**
     * @enum GKC_EventCategory
     * @brief Category of events
     */
    enum GKC_EventCategory {
        None            = 0,
        Application     = 1 << 0,
        Input           = 1 << 1,
        Physics         = 1 << 2
    };
}

namespace Galaktic::Core::Events {
    /**
     * @class GKC_Event
     * @brief Base class for events
     *
     *  Is the base class of any event in Galaktic, any event type should inherit from this class
     *  so it can work correctly, for retrieving information of an event there are 3 functions that
     *  returns information about an event: \c GetEventType, \c GetName and \c GetCategoryFlags.
     *  If you want that information you need to override the mentioned functions, for automatization
     *  in this process use a macro called \c GKC_WRITE_EVENT_FUNCS, it will override all of these for you
     */
    class GKC_Event {
        public:
            bool m_handled = false;

            virtual ~GKC_Event() = default;
            [[nodiscard]] virtual GKC_EventType GetEventType() const = 0;
            [[nodiscard]] virtual const char* GetName() const = 0;
            [[nodiscard]] virtual int GetCategoryFlags() const = 0;

            [[nodiscard]] bool IsInCategory(GKC_EventCategory category) const {
                return GetCategoryFlags() & category;
            }
    };
}

// ###################################
// ### Key Events
// ###################################

namespace Galaktic::Core::Events {
    /**
     * @class KeyPressedEvent
     * @brief A key pressed event
     */
    class KeyPressedEvent final : public GKC_Event {
        public:
            explicit KeyPressedEvent(Uint32 keycode) : m_keycode(keycode) {}
            GKC_WRITE_EVENT_FUNCS(KeyPressed, "KeyPressed", Input);

            [[nodiscard]] Uint32 GetKeyCode() const { return m_keycode; }
        private:
            Uint32 m_keycode;
    };

    /**
     * @class KeyHoldEvent
     * @brief A key hold event
     */
    class KeyHoldEvent final : public GKC_Event {
        public:
            explicit KeyHoldEvent(Uint32 keycode) : m_keycode(keycode) {}
            GKC_WRITE_EVENT_FUNCS(KeyHold, "KeyHold", Input);

            [[nodiscard]] Uint32 GetKeyCode() const { return m_keycode; }
        private:
            Uint32 m_keycode;
    };
}

// ###################################
// ### Mouse Events
// ###################################

namespace Galaktic::Core::Events {
    /**
     * @class MouseClickEvent
     * @brief A mouse click event
     */
    class MouseClickEvent final : public GKC_Event {
        public:
            explicit MouseClickEvent(float x, float y, Uint32 button) : m_x(x), m_y(y), m_button(button) {}
            GKC_WRITE_EVENT_FUNCS(MouseClick, "MouseClick", Input);

            [[nodiscard]] float GetX() const { return m_x; }
            [[nodiscard]] float GetY() const { return m_y; }
            [[nodiscard]] Uint32 GetButton() const { return m_button; }
        private:
            float m_x;
            float m_y;
            Uint32 m_button;
    };
}

// ###################################
// ### Window Events
// ###################################
namespace Galaktic::Core::Events {
    /**
     * @class WindowResizeEvent
     * @brief A window resize event
     */
    class WindowResizeEvent final : public GKC_Event {
        public:
            explicit WindowResizeEvent(Uint32 w, Uint32 h, GKC_WindowID id)
                : m_width(w), m_height(h), m_ID(id) {}
            GKC_WRITE_EVENT_FUNCS(WindowResize, "WindowResize", Application);
            [[nodiscard]] Uint32 GetWidth() const { return m_width; }
            [[nodiscard]] Uint32 GetHeight() const { return m_height; }
            [[nodiscard]] GKC_WindowID GetWindowID() const { return m_ID; }
        private:
            Uint32 m_width;
            Uint32 m_height;
            GKC_WindowID m_ID;
    };

    /**
     * @class WindowCloseEvent
     * @brief A window close event
     */
    class WindowCloseEvent final : public GKC_Event {
        public:
            explicit WindowCloseEvent(GKC_WindowID id) : m_ID(id) {}
            GKC_WRITE_EVENT_FUNCS(WindowClose, "WindowCloseEvent", Application);
            [[nodiscard]] GKC_WindowID GetWindowID() const { return m_ID; }
        private:
            GKC_WindowID m_ID;
    };

    /**
     * @class WindowMinimizedEvent
     * @brief A window minimized event
     */
    class WindowMinimizedEvent final : public GKC_Event {
        public:
            explicit WindowMinimizedEvent(GKC_WindowID id) : m_ID(id) {}
            GKC_WRITE_EVENT_FUNCS(WindowMinimized, "WindowMinimized", Application);
            [[nodiscard]] GKC_WindowID GetWindowID() const { return m_ID; }
        private:
            GKC_WindowID m_ID;
    };
}


#ifdef GKC_EVENT_DEBUG
    #define GKC_DEBUG_EVENT(...) GKC_ENGINE_INFO(__VA_ARGS__)
#else
    #define GKC_DEBUG_EVENT (void(0))
#endif