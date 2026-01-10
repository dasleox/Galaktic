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

namespace Galaktic::Core::Events {
    class GKC_Event;
}

using EventCallback = function<void(Galaktic::Core::Events::GKC_Event&)>;

namespace Galaktic::Render {
    /**
     * @enum Window_Type
     */
    enum class Window_Type {
        Fullscreen,
        Borderless,
        Transparent,
        Resizable,
        Hidden,
        OpenGL
    };

    /**
     * @class Window
     * @brief Wrapper that contains the SDL window and renderer
     * @note To call PollEvents() properly, assign the window callback using the Scene's OnEvent function
     *       before using events
     * @see gkc_scene.h and gkc_event.h for more information
     */
    class Window {
        public:
            /**
             * @param title Title of the window
             * @param width Width of the window
             * @param height Height of the window
             * @param type Type of the window
             */
            Window(const string& title, Uint32 width, Uint32 height, Window_Type type);

            ~Window();

            /**
             * @brief Draws the window
             * @param renderer
             */
            void Draw(SDL_Renderer* renderer) const;

            [[nodiscard]] string GetTitle() const { return m_title; }
            [[nodiscard]] SDL_WindowID GetWindowId() const { return m_ID; }
            [[nodiscard]] Uint32 GetWidth() const { return m_width; }
            [[nodiscard]] Uint32 GetHeight() const { return m_height; }
            [[nodiscard]] Window_Type GetType() const { return m_type; }
            [[nodiscard]] SDL_Window* GetWindow() const { return m_window; }
            [[nodiscard]] SDL_Renderer* GetRenderer() const { return m_renderer; }
            [[nodiscard]] bool ShouldClose() const { return m_requestedClose; }

            void SetTitle(const string& title) { m_title = title; }
            void SetWidth(Uint32 width) { m_width = width; }
            void SetHeight(Uint32 height) { m_height = height; }
            void SetType(Window_Type type) { m_type = type; }
            void SetCallback(const EventCallback& callback) { m_callback = callback; }

            static void SetFullScreen() {
                if (m_isFullscreen)
                    m_isFullscreen = false;
                else
                    m_isFullscreen = true;
            }
            /**
             * @brief Polls events using an SDL_Event changing the callback depending on the
             *        event captured by SDL_Event (e.g A key press)
             */
            void PollEvents();

            /**
             * @brief Requests to close the current window
             */
            void RequestClose();
        private:
            string m_title;
            SDL_WindowID m_ID = 0;
            Uint32 m_width = 0;
            Uint32 m_height = 0;
            Window_Type m_type;
            SDL_Window* m_window = nullptr;
            SDL_Renderer* m_renderer = nullptr;
            EventCallback m_callback = nullptr;
            bool m_requestedClose = false;
            static bool m_isFullscreen;

            /**
             * @brief Translates the Window_Type to an SDL_WindowFlags flag used to create an SDL_Window
             * @param type type of the window
             * @return An SDL_WindowFlags flag
             */
            SDL_WindowFlags TranslateSDLType(const Window_Type& type);

            /**
             * @brief Checks for mouse events and assign the type to a GKC_Event type and assign that to
             *        the window's callback
             * @param e SDL_Event
             */
            void SDL_MouseEventCheck(SDL_Event& e);

            /**
             * @brief Checks for key events and assign the type to a GKC_Event type and assign that to
             *        the window's callback
             * @param e SDL_Event
             */
            void SDL_KeyboardEventCheck(SDL_Event& e);

            /**
             * @brief Checks for window events and assign the type to a GKC_Event type and assign that to
             *        the window's callback
             * @param e SDL_Event
             */
            void SDL_WindowEventCheck(SDL_Event& e);
    };
}

#define GKC_GET_RENDERER(window) window->GetRenderer()
#define GKC_GET_WINDOW(window) window->GetWindow()