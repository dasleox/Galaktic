/*
  Galaktic Engine
  Copyright (C) 2025 Leonardo

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

#ifndef GKC_WINDOW_H
#define GKC_WINDOW_H

#include <pch.hpp>

using std::string;

#define GKC_WINDOW_DEF_COLOR 0,0,0,0

namespace Galaktic {
    namespace Renderer {
		class Window {
			public:
				Window(string& title, Uint16 width, Uint16 height);
				SDL_Window* GetWindow() { return window_; }

				void DrawWindow(SDL_Renderer* renderer);
				void ChangeWindowColor(SDL_Color* color);
			private:
				string title_;
				Uint16 width_;
				Uint16 height_;
				SDL_Window* window_ = nullptr;
		};
    }
}

#define GKC_GET_WINDOW(window) window->GetWindow()
#endif