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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
::Galaktic::Debug::Logger::GetClientLogger()->info(__VA_ARGS__)

#define GKC_CLIENT_WARNING(...) \
::Galaktic::Debug::Logger::GetClientLogger()->warn(__VA_ARGS__)

#define GKC_CLIENT_ERROR(...)   \
::Galaktic::Debug::Logger::GetClientLogger()->error(__VA_ARGS__)

#define GKC_CLIENT_FATAL(...)   \
::Galaktic::Debug::Logger::GetClientLogger()->critical(__VA_ARGS__)
namespace Galaktic
{
    enum class ErrorType;
    constexpr const char* ErrorTypeToDisplayString(ErrorType error);
}

namespace Galaktic::Debug {
    struct ConsoleLine
    {
        string m_text;
        spdlog::level::level_enum m_level;
    };

    template <typename Mutex>
    class ConsoleMutex : public spdlog::sinks::base_sink<Mutex>
    {
        private:
            vector<ConsoleLine> m_lines;
        protected:
            void sink_it_(const spdlog::details::log_msg& msg) override
            {
                spdlog::memory_buf_t formatted;
                spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

                ConsoleLine line;
                line.m_text  = fmt::to_string(formatted);
                line.m_level = msg.level;

                m_lines.push_back(line);
                if (m_lines.size() > m_MAXLINES) 
                {
                    m_lines.erase(m_lines.begin());
                }
            }
            void flush_() override {} 
        public:
            static constexpr size_t m_MAXLINES = 256;
            const vector<ConsoleLine>& GetLines() const { return m_lines; }
            void Clear() { m_lines.clear(); };
    };
    using ConsoleMutex_mt = ConsoleMutex<std::mutex>;

    /**
     * Main class to log anything, this class has to be initialized first before using
     * any macro (e.g. GKC_ENGINE_ERROR)
     */
    class Logger {            
        private:
            static shared_ptr<spdlog::logger> m_engineLogger;
            static shared_ptr<spdlog::logger> m_clientLogger;
            static shared_ptr<ConsoleMutex_mt> m_mutexSink;
        public:
            static shared_ptr<spdlog::logger>& GetEngineLogger()   { return m_engineLogger; }
            static shared_ptr<spdlog::logger>& GetClientLogger()   { return m_clientLogger; }
            static shared_ptr<ConsoleMutex_mt>& GetConsoleMutex()     { return m_mutexSink;    }

            /**
             * @brief Inits the loggers & sets a pattern for them
             */
            static void Init();

            /**
             * @brief Prints engine information using std::cout
             */
            static void PrintEngineInformation();
        
            static void LogInfoLua(const string& msg);
            static void LogWarningLua(const string& msg);
            static void LogErrorLua(const string& msg);

            template <typename ...Args>
            static void LogErrorWithType(ErrorType type, const string& msg, Args&&... args)
            {
                GetEngineLogger()->log(
                    spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                    spdlog::level::err,
                    fmt::runtime(msg + ErrorTypeToDisplayString(type)), 
                    std::forward<Args>(args)...
                );
            }
            template <typename ...Args>
            static void LogFatalWithType(ErrorType type, const string& msg, Args&&... args)
            {
                GetEngineLogger()->log(
                    spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                    spdlog::level::critical,
                    fmt::runtime(msg + ErrorTypeToDisplayString(type)), 
                    std::forward<Args>(args)...
                );
            }

    };
}

#if GKC_DEBUG
#define GKC_ENGINE_DEBUG(...) \
::Galaktic::Debug::Logger::GetEngineLogger()->log( \
spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, \
spdlog::level::trace, __VA_ARGS__)
#else
    #define GKC_ENGINE_DEBUG() ((void)0)
#endif

#define GKC_CHECK_PATH_INTEGRITY(path)                                \
    do {                                                               \
        if ((path).empty() ||                                          \
            !Galaktic::Filesystem::CheckFile((path))) {                \
            GKC_ENGINE_ERROR("Path: {} doesn't exist! {}",                \
                              (path).string(), (Galaktic::ErrorTypeToDisplayString(Galaktic::ErrorType::FileNotFound)));                        \
            return;                                                    \
        }                                                              \
    } while (0)

#define GKC_CHECK_PATH_INTEGRITY_BOOL(path)                                \
    do {                                                               \
        if ((path).empty() ||                                          \
            !Galaktic::Filesystem::CheckFile((path))) {                \
            GKC_ENGINE_ERROR("Path: {} doesn't exist! {}",                \
                              (path).string(), (Galaktic::ErrorTypeToDisplayString(Galaktic::ErrorType::FileNotFound)));                        \
            return false;                                                    \
        }                                                              \
    } while (0)