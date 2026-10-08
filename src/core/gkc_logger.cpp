#include <core/gkc_logger.h>
#include <script/gkc_library.h>
#include <core/gkc_error.h>

using namespace Galaktic::Debug;

namespace Galaktic::Debug {
    shared_ptr<spdlog::logger> Logger::m_engineLogger;
    shared_ptr<spdlog::logger> Logger::m_clientLogger;
    shared_ptr<ConsoleMutex_mt> Logger::m_mutexSink;
}

void Logger::Init() {
    m_mutexSink = make_shared<ConsoleMutex_mt>();

    // Every logger writes to its own stdout sink (terminal output) AND the
    // shared in-game console sink, so GKC_ENGINE_*/GKC_CLIENT_* output shows
    // up both in the terminal and inside Console::Display(). Previously
    // `spdlog::sinks_init_list(m_mutexSink)` built a throwaway temporary and
    // never actually attached the sink to a logger, so the in-game console
    // never received any lines at all.
    auto engineColorSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto clientColorSink = make_shared<spdlog::sinks::stdout_color_sink_mt>();

    m_engineLogger = make_shared<spdlog::logger>("GALAKTIC",
        spdlog::sinks_init_list{engineColorSink, m_mutexSink});
    m_clientLogger = make_shared<spdlog::logger>("CLIENT",
        spdlog::sinks_init_list{clientColorSink, m_mutexSink});

    spdlog::register_logger(m_engineLogger);
    spdlog::register_logger(m_clientLogger);

    #if GKC_DEBUG
        // Engine logger: shows file/line/function
        m_engineLogger->set_pattern("%^[%l]%$ [%D--%X] [%s -> %! || Line: %#] \033[1;33m [%n]\033[0m: %v ");
    #else
        m_engineLogger->set_pattern("%^[%l]%$ [%D--%X] \033[1;33m [%n]\033[0m: %v ");
    #endif

    // Client logger: simple format with [Lua]: prefix
    m_clientLogger->set_pattern("%^[%l]%$ [%D--%X]%v ");
}

void Logger::PrintEngineInformation() {
    cout << "Galaktic Engine " << GKC_VERSION_STR << " '"
        << GKC_SUFFIX << "' " << "Build " << GKC_BUILD_VERSION << endl;
}

void Logger::LogInfoLua(const string& msg) {
    auto luaContext = Galaktic::Script::LuaGalaktic::GetLuaCallContext(1);
    GKC_CLIENT_INFO(" {} \033[1;34m[LUA]\033[0m: {}", luaContext, msg);
}

void Logger::LogWarningLua(const string& msg) {
    auto luaContext = Galaktic::Script::LuaGalaktic::GetLuaCallContext(1);
    GKC_CLIENT_WARNING(" {} \033[1;34m[LUA]\033[0m: {}", luaContext, msg);
}

void Logger::LogErrorLua(const string& msg) {
    auto luaContext = Galaktic::Script::LuaGalaktic::GetLuaCallContext(1);
    GKC_CLIENT_ERROR(" {} \033[1;34m[LUA]\033[0m: {}", luaContext, msg);
}