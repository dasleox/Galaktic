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

namespace Galaktic::Debug {
    /**
     * @brief Contains a logger for Client & Engine
     * @note Init() function has to be called first before using the loggers
     */
    class Logger {
        public:
            /**
             * @brief Inits the loggers & sets a pattern for them
             */
            static void Init();

            /**
             * @brief Prints engine information
             */
            static void PrintEngineInformation();

            /**
             * @brief Gets the engine version
             * @return A string with the engine version
             */
            static string GetEngineName();

            /**
             * @brief Returns a string with the display information
             * FORMAT: Width x Height @ (Hertz) Hz
             * @param deviceInfo DeviceInformation instance
             * @return A string with the display information
             */
            static string GetDisplayInfo(const Core::DeviceInformation& deviceInfo);

            /**
             * @brief Demangle a typename
             * @param name Typename
             * @return name of the typename
             */
            static string DemangleTypename(const char* name);

            static shared_ptr<spdlog::logger>& GetEngineLogger()   { return engine_logger_; }
            static shared_ptr<spdlog::logger>& GetClientLogger()   { return client_logger_; }
        private:
            static shared_ptr<spdlog::logger> engine_logger_;
            static shared_ptr<spdlog::logger> client_logger_;
    };
}

#define GKC_ENGINE_INFO(...)    \
::Galaktic::Debug::Logger::GetEngineLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::info, __VA_ARGS__)

#define GKC_ENGINE_WARNING(...) \
::Galaktic::Debug::Logger::GetEngineLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::warn, __VA_ARGS__)

#define GKC_ENGINE_ERROR(...)   \
::Galaktic::Debug::Logger::GetEngineLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::err, __VA_ARGS__)

#define GKC_ENGINE_FATAL(...)   \
::Galaktic::Debug::Logger::GetEngineLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::critical, __VA_ARGS__)


// Client-level logging macros

#define GKC_CLIENT_INFO(...)    \
::Galaktic::Debug::Logger::GetClientLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::info, __VA_ARGS__)

#define GKC_CLIENT_WARNING(...) \
::Galaktic::Debug::Logger::GetClientLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::warn, __VA_ARGS__)

#define GKC_CLIENT_ERROR(...)   \
::Galaktic::Debug::Logger::GetClientLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::err, __VA_ARGS__)

#define GKC_CLIENT_FATAL(...)   \
::Galaktic::Debug::Logger::GetClientLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::critical, __VA_ARGS__)


#ifdef GKC_PRINT_ADDRESSES
    /**
     * @brief Prints the address of the given pointer/variable
     * @param ptr Pointer/Variable
     * @param name Name of the variable
     * @note Sometimes you may need to cast the ptr to a void pointer
     *       e.g (void*) ptr, in order to print!
     */
    #define GKC_PRINT_ADDRESS(name, ptr) GKC_ENGINE_INFO("Address '{0}': {1}", name, static_cast<void*>(&ptr))
#else
    #define GKC_PRINT_ADDRESS (void(0))
#endif

