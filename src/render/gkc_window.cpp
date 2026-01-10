#include <render/gkc_window.h>
#include <core/gkc_logger.h>
#include <core/events/gkc_event.h>

using namespace Galaktic::Render;
bool Window::m_isFullscreen = false;

Window::Window(const string& title, Uint32 width, Uint32 height, Window_Type type)
    : m_title(title), m_width(width), m_height(height), m_type(type) {

    // Dont worry about unsigned values here
    m_window = SDL_CreateWindow(title.c_str(), width, height, TranslateSDLType(type) |
        SDL_WINDOW_RESIZABLE);
    m_renderer = SDL_CreateRenderer(m_window, NULL);

    GKC_ASSERT(m_renderer != nullptr, "Renderer wasn't created correctly!");
    GKC_ASSERT(m_window != nullptr, "Window wasn't created correctly!");

    if (TranslateSDLType(type) == SDL_WINDOW_RESIZABLE || SDL_WINDOW_OPENGL) {
        SDL_MaximizeWindow(m_window);
    }

    SDL_SyncWindow(m_window);
    m_ID = SDL_GetWindowID(m_window);
    GKC_ENGINE_INFO("'{0}' window built successfully!", title);
}

Window::~Window() {
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    GKC_ENGINE_INFO("'{0}' window has been destroyed!", m_title);
}

void Window::Draw(SDL_Renderer* renderer) const {
    SDL_SetWindowFullscreen(m_window, m_isFullscreen);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void Window::PollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        SDL_WindowEventCheck(e);
        SDL_KeyboardEventCheck(e);
        SDL_MouseEventCheck(e);
    }
}

void Window::RequestClose() {
    GKC_ENGINE_INFO("Requested close for: '{0}' [ID: {1}]", m_title, m_ID);
    m_requestedClose = true;
}

SDL_WindowFlags Window::TranslateSDLType(const Window_Type& type) {
    switch (type) {
        case Window_Type::Fullscreen:
            m_isFullscreen = true;
            return SDL_WINDOW_FULLSCREEN;
        case Window_Type::Borderless:
            return SDL_WINDOW_BORDERLESS;
        case Window_Type::Transparent:
            return SDL_WINDOW_TRANSPARENT;
        case Window_Type::Hidden:
            return SDL_WINDOW_HIDDEN;
        case Window_Type::OpenGL:
            return SDL_WINDOW_OPENGL;
        default:
            return SDL_WINDOW_RESIZABLE;
    }
}

void Window::SDL_KeyboardEventCheck(SDL_Event &e) {
    using namespace Galaktic::Core::Events;
    switch (e.type) {
        case SDL_EVENT_KEY_DOWN: {
            KeyPressedEvent event(e.key.key);
            m_callback(event); break;
        }
        case SDL_EVENT_KEY_UP: {
            KeyHoldEvent event(e.key.key);
            m_callback(event); break;
        }
    }
}

void Window::SDL_WindowEventCheck(SDL_Event &e) {
    using namespace Galaktic::Core::Events;
    switch (e.type) {
        case SDL_EVENT_WINDOW_RESIZED: {
            WindowResizeEvent event(e.window.data1, e.window.data2, e.window.windowID);
            m_callback(event); break;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
            WindowCloseEvent event(e.window.windowID);
            m_callback(event); break;
        }
    }
}

void Window::SDL_MouseEventCheck(SDL_Event &e) {
    using namespace Galaktic::Core::Events;
    switch (e.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            MouseClickEvent event(e.motion.x, e.motion.y, e.button.button);
            m_callback(event); break;
        }
    }
}
