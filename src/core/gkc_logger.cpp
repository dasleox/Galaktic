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
    spdlog::sinks_init_list(m_mutexSink);

    #if GKC_DEBUG
        // Engine logger: shows file/line/function
        spdlog::set_pattern("%^[%l]%$ [%D--%X] [%s -> %! || Line: %#] \033[1;33m [%n]\033[0m: %v ");
        
        m_engineLogger = spdlog::stdout_color_mt("GALAKTIC");
        
        // Client logger: simple format with [Lua]: prefix

        m_clientLogger = spdlog::stdout_color_mt("CLIENT");
        m_clientLogger->set_pattern("%^[%l]%$ [%D--%X]%v ");
    #else
        spdlog::set_pattern("%^[%l]%$ [%D--%X] \033[1;33m [%n]\033[0m: %v ");
        m_engineLogger = spdlog::stdout_color_mt("GALAKTIC");
        
        m_clientLogger = spdlog::stdout_color_mt("CLIENT");
        m_clientLogger->set_pattern("%^[%l]%$ [%D--%X] %v ");
    #endif
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