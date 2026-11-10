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
    /**
     * @brief Returns the diagnostic name corresponding to an engine error category.
     * @param error Engine error category
     */
    constexpr const char* ErrorTypeToDisplayString(ErrorType error);
}

namespace Galaktic::Debug {
    /**
     * @struct ConsoleLine
     * @brief Stores a formatted log message, severity and SDL timestamp for console display.
     */
    struct ConsoleLine
    {
        std::string text;
        spdlog::level::level_enum level;
        Uint64 timestamp = 0;
    };
    /**
     * @class ConsoleMutex
     * @brief Collects recent log messages through spdlog's synchronized sink interface.
     */
    template <typename Mutex>
    class ConsoleMutex : public spdlog::sinks::base_sink<Mutex>
    {
        private:
            std::vector<ConsoleLine> m_lines;
        protected:
            /**
             * @brief Formats and appends a log line, discarding the oldest lines when the console limit is reached.
             * @param msg Log message or format string
             */
            void sink_it_(const spdlog::details::log_msg& msg) override
            {
                spdlog::memory_buf_t formatted;
                spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

                ConsoleLine line;
                line.text  = fmt::to_string(formatted);
                line.level = msg.level;

                m_lines.push_back(line);
                if (m_lines.size() > m_MAXLINES) 
                {
                    m_lines.erase(m_lines.begin());
                }
            }
            /**
             * @brief Implements the in-memory sink's no-op flush hook.
             */
            void flush_() override {} 
        public:
            static constexpr size_t m_MAXLINES = 256;
            /**
             * @brief Returns borrowed chronological log lines; the sink owns their storage.
             */
            const std::vector<ConsoleLine>& GetLines() const { return m_lines; }
            /**
             * @brief Clears the stored entries.
             */
            void Clear() { m_lines.clear(); };
    };
    using ConsoleMutex_mt = ConsoleMutex<std::mutex>;

    /**
     * Main class to log anything, this class has to be initialized first before using
     * any macro (e.g. GKC_ENGINE_ERROR)
     */
    class Logger {            
        private:
            static std::shared_ptr<spdlog::logger> m_engineLogger;
            static std::shared_ptr<spdlog::logger> m_clientLogger;
            static std::shared_ptr<ConsoleMutex_mt> m_mutexSink;
        public:
            /**
             * @brief Returns access to the shared engine logger initialized by Logger::Init.
             */
            static std::shared_ptr<spdlog::logger>& GetEngineLogger()   { return m_engineLogger; }
            /**
             * @brief Returns access to the shared client logger initialized by Logger::Init.
             */
            static std::shared_ptr<spdlog::logger>& GetClientLogger()   { return m_clientLogger; }
            /**
             * @brief Returns access to the shared synchronized console log sink.
             */
            static std::shared_ptr<ConsoleMutex_mt>& GetConsoleMutex()     { return m_mutexSink;    }

            /**
             * @brief Inits the loggers & sets a pattern for them
             */
            static void Init();

            /**
             * @brief Prints engine information using std::cout
             */
            static void PrintEngineInformation();
        
            /**
             * @brief Logs an informational Lua message with its script call context.
             * @param msg Log message or format string
             */
            static void LogInfoLua(const std::string& msg);
            /**
             * @brief Logs a Lua warning with its script call context.
             * @param msg Log message or format string
             */
            static void LogWarningLua(const std::string& msg);
            /**
             * @brief Logs a Lua error with its script call context.
             * @param msg Log message or format string
             */
            static void LogErrorLua(const std::string& msg);

            /**
             * @brief Formats an error message and appends its engine error category.
             * @param type Engine type identifier used by this operation
             * @param msg Log message or format string
             * @param args Arguments forwarded to the requested operation
             */
            template <typename ...Args>
            static void LogErrorWithType(ErrorType type, const std::string& msg, Args&&... args)
            {
                GetEngineLogger()->log(
                    spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION},
                    spdlog::level::err,
                    fmt::runtime(msg + ErrorTypeToDisplayString(type)), 
                    std::forward<Args>(args)...
                );
            }
            /**
             * @brief Formats a critical error message and appends its engine error category.
             * @param type Engine type identifier used by this operation
             * @param msg Log message or format string
             * @param args Arguments forwarded to the requested operation
             */
            template <typename ...Args>
            static void LogFatalWithType(ErrorType type, const std::string& msg, Args&&... args)
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

#define GKC_FILE_ALREADY_EXISTS_BOOL(filepath)                                  \
    do {                                                               \
        if (::Galaktic::Filesystem::CheckFile((filepath))) {                                       \
            Debug::Logger::LogErrorWithType(ErrorType::FileAlreadyExists, "{} already exists!", (filepath.string())); \
            return false;                                                                                           \
        }                                                                                                           \
    } while (0)
