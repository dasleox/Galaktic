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

#ifndef GKC_MAIN_H
#define GKC_MAIN_H

#define GKC_VERSION "0.0a"
#define GKC_SUFFIX "Earthy"
#define GKC_BUILD_VERSION 1

// Operating System Detection

#if __WIN64
#define GKC_OS "Windows"
#define GKC_OS_INT 0
#elif __linux__
#define GKC_OS "Linux"
#define GKC_OS_INT 1
#elif __APPLE__
#define GKC_OS "Apple"
#define GKC_OS_INT 2
#endif

// Architecture detection

#if __i386__ || __WIN32
#error "32-bit architecture is not supported in the Galaktic Engine!"
#elif __amd64__ || __x86_64__
#define GKC_ARCH "x64"
#else
#error "The Galaktic Engine does not support the current architecture!"
#endif

#define C_ARRAY_MAX_BUFFER 128

#include <pch.hpp>

using std::shared_ptr, std::unique_ptr, std::cout, std::cin, std::endl;
using std::string, std::vector;

namespace Galaktic {

    namespace Debug {
        extern void PrintEngineInformation();
    }

    namespace Core {
        /**
         * @struct
         * @brief Struct used for saving the device information, like the width and height of the screen
         *        the OS and the CPU architecture
         */
        struct DeviceInformation {
            Uint16 width_;
            Uint16 height_;
            string os_;
            string arch_;
        };
    }
}

// Engine-level logging macros
#define GKC_ENGINE_INFO(...)    ::Galaktic::Debug::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define GKC_ENGINE_WARNING(...) ::Galaktic::Debug::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define GKC_ENGINE_ERROR(...)   ::Galaktic::Debug::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define GKC_ENGINE_FATAL(...)   ::Galaktic::Debug::Logger::GetEngineLogger()->critical(__VA_ARGS__)

// Client-level logging macros
#define GKC_CLIENT_INFO(...)    ::Galaktic::Debug::Logger::GetClientLogger()->info(__VA_ARGS__)
#define GKC_CLIENT_WARNING(...) ::Galaktic::Debug::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define GKC_CLIENT_ERROR(...)   ::Galaktic::Debug::Logger::GetClientLogger()->error(__VA_ARGS__)
#define GKC_CLIENT_FATAL(...)   ::Galaktic::Debug::Logger::GetClientLogger()->critical(__VA_ARGS__)

#endif