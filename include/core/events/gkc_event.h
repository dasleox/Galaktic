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

namespace Galaktic::Core {
    class Scene;
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
        KeyUp,
        MouseClick,
        MouseWheel,
        WindowResize,
        WindowClose,
        WindowMinimized,
        ObjectCreated, 
        EntityCreated, 
        EntityDeleted, 
        EntityModified,
        ScriptCalledFromEntity,
        ScriptCalled,
        TextInput
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

            /**
             * @brief Destroys this object; borrowed entities and external resources remain owned by their callers.
             */
            virtual ~GKC_Event() = default;
            /**
             * @brief Returns the event's engine type identifier.
             */
            [[nodiscard]] virtual GKC_EventType GetEventType() const = 0;
            /**
             * @brief Returns the name identifying this object or event.
             */
            [[nodiscard]] virtual const char* GetName() const = 0;
            /**
             * @brief Returns the event's category bitmask.
             */
            [[nodiscard]] virtual int GetCategoryFlags() const = 0;

            /**
             * @brief Checks whether the event belongs to the requested category.
             * @param category Category
             * @return true if the event belongs to the requested category, false otherwise
             */
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
            /**
             * @brief Creates the event with the supplied payload values.
             * @param keycode SDL keyboard key code
             */
            explicit KeyPressedEvent(Uint32 keycode) : m_keycode(keycode) {}
            GKC_WRITE_EVENT_FUNCS(KeyPressed, "KeyPressed", Input);

            /**
             * @brief Returns the SDL key code carried by the event.
             */
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
            /**
             * @brief Creates the event with the supplied payload values.
             * @param keycode SDL keyboard key code
             */
            explicit KeyHoldEvent(Uint32 keycode) : m_keycode(keycode) {}
            GKC_WRITE_EVENT_FUNCS(KeyHold, "KeyHold", Input);

            /**
             * @brief Returns the SDL key code carried by the event.
             */
            [[nodiscard]] Uint32 GetKeyCode() const { return m_keycode; }
        private:
            Uint32 m_keycode;
    };

    /**
     * @class KeyHoldEvent
     * @brief A key hold event
     */
    class KeyUpEvent final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param keycode SDL keyboard key code
             */
            explicit KeyUpEvent(Uint32 keycode) : m_keycode(keycode) {}
            GKC_WRITE_EVENT_FUNCS(KeyUp, "KeyUp", Input);

            /**
             * @brief Returns the SDL key code carried by the event.
             */
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
            /**
             * @brief Creates the event with the supplied payload values.
             * @param x Horizontal coordinate or amount
             * @param y Vertical coordinate or amount
             * @param button SDL mouse button number
             * @param pressed true for a button press, false for a release
             */
            explicit MouseClickEvent(float x, float y, Uint32 button, bool pressed = true)
                : m_x(x), m_y(y), m_button(button), m_pressed(pressed) {}
            GKC_WRITE_EVENT_FUNCS(MouseClick, "MouseClick", Input);

            /**
             * @brief Returns the horizontal event coordinate or mouse-wheel amount.
             */
            [[nodiscard]] float GetX() const { return m_x; }
            /**
             * @brief Returns the vertical event coordinate or mouse-wheel amount.
             */
            [[nodiscard]] float GetY() const { return m_y; }
            /**
             * @brief Returns the SDL mouse button number carried by the event.
             */
            [[nodiscard]] Uint32 GetButton() const { return m_button; }
            /**
             * @brief Checks whether the mouse event represents a press rather than a release.
             * @return true if the mouse event represents a press rather than a release, false otherwise
             */
            [[nodiscard]] bool IsPressed() const { return m_pressed; }
        private:
            float m_x;
            float m_y;
            Uint32 m_button;
            bool m_pressed;
    };

    /**
     * @class MouseWheelEvent
     * @brief Carries horizontal and vertical mouse-wheel movement.
     */
    class MouseWheelEvent final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param x Horizontal coordinate or amount
             * @param y Vertical coordinate or amount
             */
            explicit MouseWheelEvent(float x, float y) : m_x(x), m_y(y) {}
            GKC_WRITE_EVENT_FUNCS(MouseWheel, "MouseWheel", Input);

            /**
             * @brief Returns the horizontal event coordinate or mouse-wheel amount.
             */
            [[nodiscard]] float GetX() const { return m_x; }
            /**
             * @brief Returns the vertical event coordinate or mouse-wheel amount.
             */
            [[nodiscard]] float GetY() const { return m_y; }
        private:
            float m_x;
            float m_y;
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
            /**
             * @brief Creates the event with the supplied payload values.
             * @param w W
             * @param h H
             * @param id Entity or asset ID used by this operation
             */
            explicit WindowResizeEvent(Uint32 w, Uint32 h, GKC_WindowID id)
                : m_width(w), m_height(h), m_ID(id) {}
            GKC_WRITE_EVENT_FUNCS(WindowResize, "WindowResize", Application);
            /**
             * @brief Returns the width in pixels.
             */
            [[nodiscard]] Uint32 GetWidth() const { return m_width; }
            /**
             * @brief Returns the height in pixels.
             */
            [[nodiscard]] Uint32 GetHeight() const { return m_height; }
            /**
             * @brief Returns the engine window ID stored in this object or event.
             */
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
            /**
             * @brief Creates the event with the supplied payload values.
             * @param id Entity or asset ID used by this operation
             */
            explicit WindowCloseEvent(GKC_WindowID id) : m_ID(id) {}
            GKC_WRITE_EVENT_FUNCS(WindowClose, "WindowCloseEvent", Application);
            /**
             * @brief Returns the engine window ID stored in this object or event.
             */
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
            /**
             * @brief Creates the event with the supplied payload values.
             * @param id Entity or asset ID used by this operation
             */
            explicit WindowMinimizedEvent(GKC_WindowID id) : m_ID(id) {}
            GKC_WRITE_EVENT_FUNCS(WindowMinimized, "WindowMinimized", Application);
            /**
             * @brief Returns the engine window ID stored in this object or event.
             */
            [[nodiscard]] GKC_WindowID GetWindowID() const { return m_ID; }
        private:
            GKC_WindowID m_ID;
    };
}


// ###################################
// ### Entity Events
// ###################################
namespace Galaktic::Core::Events {
    /**
     * @class EntityCreatedEvent
     * @brief Requests creation of a named entity with the specified tag type.
     */
    class EntityCreatedEvent final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param name Name used to identify the requested entry
             * @param type Engine type identifier used by this operation
             */
            explicit EntityCreatedEvent(const std::string& name, const std::type_index& type) :
                m_name(name), m_type(type) {}
            GKC_WRITE_EVENT_FUNCS(EntityCreated, "EntityCreated", Application);
            /**
             * @brief Returns the entity name carried by the event.
             */
            [[nodiscard]] std::string& GetEntityName() { return m_name; }
            /**
             * @brief Returns the tag or component type carried by the event.
             */
            [[nodiscard]] std::type_index GetEntityType() { return m_type; }
        private:
            std::string m_name;
            std::type_index m_type;
    };

    /**
     * @class EntityModifiedEvent
     * @brief Carries an entity ID, component type and replacement component value.
     */
    class EntityModifiedEvent final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param id Entity or asset ID used by this operation
             * @param type Engine type identifier used by this operation
             * @param comp Component value to serialize or populate
             */
            explicit EntityModifiedEvent(EntityID id, const std::type_index& type, std::any&& comp) :
                m_id(id), m_type(type), m_comp(comp) {}
            GKC_WRITE_EVENT_FUNCS(EntityModified, "EntityModified", Application);
            /**
             * @brief Returns the entity ID carried by the event.
             */
            [[nodiscard]] EntityID GetEntityID() { return m_id; }
            /**
             * @brief Returns the tag or component type carried by the event.
             */
            [[nodiscard]] std::type_index GetEntityType() { return m_type; }
            /**
             * @brief Returns a copy of the replacement component carried by the event.
             */
            [[nodiscard]] std::any GetEntityComponent() { return m_comp; }
        private:
            EntityID m_id;
            std::type_index m_type;
            std::any m_comp;
    };


    /**
     * @class EntityDestroyedEvent
     * @brief Requests deletion of an entity by ID.
     */
    class EntityDestroyedEvent final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param id Entity or asset ID used by this operation
             */
            explicit EntityDestroyedEvent(EntityID id) : m_id(id) {}
            GKC_WRITE_EVENT_FUNCS(EntityDeleted, "EntityDeleted", Application);
            /**
             * @brief Returns the entity ID carried by the event.
             */
            [[nodiscard]] EntityID GetEntityID() { return m_id; }
        private:
            EntityID m_id;
    };
}

// ###################################
// ### Script Events
// ###################################
namespace Galaktic::Core::Events {
    /**
     * @class ScriptCalledFromEntity
     * @brief Requests execution of the script associated with an entity.
     */
    class ScriptCalledFromEntity final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param id Entity or asset ID used by this operation
             */
            explicit ScriptCalledFromEntity(EntityID id) : m_id(id) {}
            GKC_WRITE_EVENT_FUNCS(ScriptCalledFromEntity, "ScriptCalledFromEntity", Application);
            /**
             * @brief Returns the entity ID carried by the event.
             */
            [[nodiscard]] EntityID GetEntityID() { return m_id; }
        private:
            EntityID m_id;
    };

    /**
     * @class ScriptCalled
     * @brief Requests execution of a registered script by ID.
     */
    class ScriptCalled final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param id Entity or asset ID used by this operation
             */
            explicit ScriptCalled(ScriptID id) : m_id(id) {}
            GKC_WRITE_EVENT_FUNCS(ScriptCalled, "ScriptCalled", Application);
            /**
             * @brief Returns the registered script ID carried by the event.
             */
            [[nodiscard]] ScriptID GetScriptID() { return m_id; }
        private:
            ScriptID m_id;
    };
}

// ###################################
// ### Text Input Events
// ###################################
namespace Galaktic::Core::Events {
    /**
     * @class TextInputEvent
     * @brief Fired for SDL's composed text input (SDL_EVENT_TEXT_INPUT).
     *
     * Carries the UTF-8 text SDL produced for this input step. Prefer this
     * over KeyPressedEvent for anything that needs actual typed characters
     * (e.g. the Console's input line), since it already accounts for
     * keyboard layout and shift/caps state.
     */
    class TextInputEvent final : public GKC_Event {
        public:
            /**
             * @brief Creates the event with the supplied payload values.
             * @param text Text string
             */
            explicit TextInputEvent(const std::string& text) : m_text(text) {}
            GKC_WRITE_EVENT_FUNCS(TextInput, "TextInput", Input);

            /**
             * @brief Returns the UTF-8 text carried by this input event.
             */
            [[nodiscard]] const std::string& GetText() const { return m_text; }
        private:
            std::string m_text;
    };
}


#ifdef GKC_EVENT_DEBUG
    #define GKC_DEBUG_EVENT(...) GKC_ENGINE_INFO(__VA_ARGS__)
#else
    #define GKC_DEBUG_EVENT (void(0))
#endif
