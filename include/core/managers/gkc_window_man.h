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
    class WindowManager {
        public:
            static void CreateGKCWindow(const string& title, Uint32 width, Uint32 height, Render::Window_Type type);
            static void RegisterWindow(shared_ptr<Render::Window> window);
            static void UnregisterWindow(GKC_WindowID id);
            static shared_ptr<Render::Window> GetWindow(GKC_WindowID id);

            static unordered_map<GKC_WindowID, shared_ptr<Render::Window>>& GetWindowList() {
                return m_windowList;
            }
        private:
            static unordered_map<GKC_WindowID, shared_ptr<Render::Window>> m_windowList;
    };

    typedef unordered_map<GKC_WindowID, shared_ptr<Render::Window>> Window_List;
}
