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

#ifndef GKC_LOGGER_H
#define GKC_LOGGER_H

#include <pch.hpp>

using std::shared_ptr;

namespace Galaktic{
namespace Debug {
    class Logger{
        public:
            static void Init();
            inline static shared_ptr<spdlog::logger>& GetEngineLogger()   { return engine_logger; }
            inline static shared_ptr<spdlog::logger>& GetClientLogger()   { return client_logger; }
        private:
            static shared_ptr<spdlog::logger> engine_logger;
            static shared_ptr<spdlog::logger> client_logger;
    };
}}

#endif