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

#ifndef GKC_RENDERER_H
#define GKC_RENDERER_H

#include <pch.hpp>
#include <core/gkc_main.h>

namespace Galaktic {
	namespace Core {
		struct AppInformation;
	}
	namespace Renderer {
		class Window;
		class Renderer {
			public:
				Renderer(Core::DeviceInformation& device_info, Core::AppInformation& app_info);
				SDL_Renderer* GetRenderer() { return renderer_; }
				Window* window_ = nullptr;
			private:
				SDL_Renderer* renderer_ = nullptr;
		};
	}
}

#define GKC_GET_RENDERER(renderer) renderer->GetRenderer()
#endif