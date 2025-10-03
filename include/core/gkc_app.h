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

#ifndef GKC_APP_H
#define GKC_APP_H

#include <pch.hpp>
#include <core/gkc_main.h>
#include <graphics/gkc_renderer.h>

using std::string;

namespace Galaktic {
namespace Core {
    enum class AppType;

    struct AppInformation {
        std::string app_name_;
        AppType app_type_;
    };

    class App {
        public:
            App(std::string title, AppType type);
            void Init();
            void Run();
            void Loop();
            void Free();
        private:
            Renderer::Renderer* renderer_;
            AppInformation app_info_; 
            DeviceInformation device_info_;
            SDL_Event event_;
    };

    enum class AppType {
        Undefined = -1,
        Game = 0,
        Application = 1,
    };
}}

#endif