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

namespace Galaktic::Render {
    class Window;
    enum class Window_Type;
}

namespace Galaktic::Core::Managers {
    /**
     * @class WindowManager
     * 
     * Manager for windows, responsible for creating, registering, and retrieving windows.
     * All of the functions and list are static, an instance of this class is not needed.
     * 
     * When a window is registered, is assigned a unique GKC_WindowID that is increased by
     * 1 for each new window created. However a limit of 256 windows is set, after which
     * the manager will not allow any new windows to be created. 
     * When a window is unregistered, its ID is freed along with its Window instance and the
     * ID can be reused for a new window.
     */
    class WindowManager {
        public:
            /**
             * @param title Title of the window
             * @param width Width of the window
             * @param height Height of the window
             * @param type Type of the window
             * @see Window_Type for types of windows that can be created
             */
            static void CreateGKCWindow(const string& title, Uint32 width, Uint32 height, Render::Window_Type type);

            /**
             * Registers a window into the list, it is automatically assigned a GKC_WindowID
             * a maximum of 256 windows can be created. Calling this function with more than
             * 256 windows will not register the window.
             * @param window shared_ptr Window
             */
            static void RegisterWindow(shared_ptr<Render::Window> window);

            /**
             * Unregisters a window from the list, its resources are freed, the destructor
             * of Window is automatically called when erased from the list.
             * @param id GKC_WindowID 
             */
            static void UnregisterWindow(GKC_WindowID id);

            /**
             * Retrieves a window from the list using its ID, if the window is not found,
             * A warning is emitted and nullptr is returned.
             * @param id GKC_WindowID
             * @return shared_ptr Window associated with the given GKC_WindowID, nullptr if not found
             */
            static shared_ptr<Render::Window> GetWindow(GKC_WindowID id);

            static unordered_map<GKC_WindowID, shared_ptr<Render::Window>>& GetWindowList() {
                return m_windowList;
            }
        private:
            static unordered_map<GKC_WindowID, shared_ptr<Render::Window>> m_windowList;
    };

    typedef unordered_map<GKC_WindowID, shared_ptr<Render::Window>> Window_List;
}
