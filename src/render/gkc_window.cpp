#include <render/gkc_window.h>
#include <core/gkc_logger.h>
#include <core/events/gkc_event.h>
#include <core/gkc_error.h>
#include <filesys/gkc_filesys.h>

using namespace Galaktic::Render;
bool Window::m_isFullscreen = false;

Window::Window(const string& title, Uint32 width, Uint32 height, Window_Type type)
    : m_title(title), m_width(width), m_height(height), m_type(type) {

    // Dont worry be happi uuuh ,about unsigned values here
    m_window = SDL_CreateWindow(title.c_str(), width, height, TranslateSDLType(type) |
        SDL_WINDOW_RESIZABLE);
    m_renderer = SDL_CreateRenderer(m_window, NULL);

    GKC_ASSERT(m_renderer != nullptr, "Renderer wasn't created correctly!");
    GKC_ASSERT(m_window != nullptr, "Window wasn't created correctly!");

    if (TranslateSDLType(type) == SDL_WINDOW_RESIZABLE) {
        SDL_MaximizeWindow(m_window);
    }

    SDL_SyncWindow(m_window);

    // The window can be maximized, so read the real size
    int realWidth = 0, realHeight = 0;
    SDL_GetWindowSize(m_window, &realWidth, &realHeight);
    m_width = static_cast<Uint32>(realWidth);
    m_height = static_cast<Uint32>(realHeight);

    m_ID = SDL_GetWindowID(m_window);
    SDL_StartTextInput(m_window);
}

Window::~Window() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    GKC_ENGINE_INFO("'{0}' window has been destroyed!", m_title);
}

void Window::Draw(SDL_Renderer* renderer) const {
    if(m_isFullscreen) {
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    } else {
        SDL_SetWindowFullscreen(m_window, 0);
    }
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void Window::PollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        SDL_ConvertEventToRenderCoordinates(m_renderer, &e);
        SDL_WindowEventCheck(e);
        SDL_KeyboardEventCheck(e);
        SDL_MouseEventCheck(e);
    }
}

void Window::RequestClose() {
    GKC_ENGINE_INFO("Requested close for: '{0}' [ID: {1}]", m_title, m_windowID);
    m_requestedClose = true;
}

bool Window::IsValid() {
     return m_window != nullptr;
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
    GKC_RELEASE_ASSERT(m_callback != nullptr, "Event callbacks are not registered, please register a callback function from the scene");

    switch (e.type) {
        case SDL_EVENT_KEY_DOWN: {
            KeyPressedEvent event(e.key.key);
            m_callback(event); break;
        }
        case SDL_EVENT_KEY_UP: {    
            KeyHoldEvent event(e.key.key);
            m_callback(event); break;
        }
        case SDL_EVENT_TEXT_INPUT: {
            TextInputEvent event(e.text.text);
            m_callback(event); break;
        }
    }
}

void Window::SDL_WindowEventCheck(SDL_Event &e) {
    using namespace Galaktic::Core::Events;
    GKC_RELEASE_ASSERT(m_callback != nullptr, "Event callbacks are not registered, please register a callback function from the scene");
    switch (e.type) {
        case SDL_EVENT_WINDOW_RESIZED: {
            WindowResizeEvent event(e.window.data1, e.window.data2, m_windowID);
            m_callback(event); break;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
            WindowCloseEvent event(m_windowID);
            m_callback(event); break;
        }
    }
}

void Window::SDL_MouseEventCheck(SDL_Event &e) {
    using namespace Galaktic::Core::Events;
    SDL_ConvertEventToRenderCoordinates(m_renderer, &e);
    GKC_RELEASE_ASSERT(m_callback != nullptr,"Event callbacks are not registered, please register a callback function from the scene");
    switch (e.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            MouseClickEvent event(e.button.x, e.button.y, e.button.button);
            m_callback(event); break;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            MouseWheelEvent event(e.wheel.x, e.wheel.y);
            m_callback(event); break;
        }
    }
}

void Galaktic::Render::Window::SetIcon(const path &iconPath) {
    if(!Filesystem::CheckFile(iconPath)) {
        Debug::Logger::LogErrorWithType(ErrorType::FileNotFound, "Icon path doesn't exist");
        return;
    }
    SDL_Surface* surface = IMG_Load(iconPath.string().c_str());
    if(surface == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError, "Failed to create icon!");
        return;
    }
    
    SDL_SetWindowIcon(m_window, surface);
    SDL_DestroySurface(surface);
}

void Window::SetPixelResolution(int width, int height) {
    if (width <= 0 || height <= 0) {
        Debug::Logger::LogErrorWithType(ErrorType::OutOfRange,
            "Pixel resolution must be bigger than 0 ({}x{})", width, height);
        return;
    }

    if (!SDL_SetRenderLogicalPresentation(m_renderer, width, height,
        SDL_LOGICAL_PRESENTATION_INTEGER_SCALE)) {
        Debug::Logger::LogErrorWithType(ErrorType::LibraryError,
            "Failed to set pixel resolution: {}", SDL_GetError());
        return;
    }

    m_virtualWidth = static_cast<Uint32>(width);
    m_virtualHeight = static_cast<Uint32>(height);
}

void Window::DisablePixelResolution() {
    SDL_SetRenderLogicalPresentation(m_renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
    m_virtualWidth = 0;
    m_virtualHeight = 0;
}