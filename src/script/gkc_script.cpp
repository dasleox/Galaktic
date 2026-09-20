#include <script/gkc_script.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>
#include <core/gkc_error.h>

using namespace Galaktic;

Script::GKC_Script::GKC_Script(const path& scriptPath, lua_State* luaState) 
    : m_scriptName(Core::StripExtension(scriptPath.filename().string())) {

    m_luaState = luaState;
    if (luaL_loadfile(m_luaState, scriptPath.string().c_str()) != LUA_OK) {
        string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 1);
        Debug::Logger::LogFatalWithType(ErrorType::ScriptCompileFailed, 
            "Failed to compile script file: {} - {}", scriptPath.string(), error);
        return;
    }

    if (lua_pcall(m_luaState, 0, 0, 0) != LUA_OK) {
        string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 1);
        Debug::Logger::LogFatalWithType(ErrorType::ScriptRuntimeError, 
            "Failed to execute script file: {} - {}", scriptPath.string(), error);
        return;
    }
}

Script::GKC_Script::GKC_Script(const string& scriptName, const string& scriptString, lua_State* luaState) 
    : m_scriptName(scriptName) {
    m_luaState = luaState;
    if (luaL_loadbuffer(m_luaState, scriptString.c_str(), scriptString.size(), 
                       scriptName.c_str()) != LUA_OK) {
        string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 1);
        Debug::Logger::LogFatalWithType(ErrorType::ScriptCompileFailed, 
            "Failed to compile script string: {} - {}", scriptName, error);
        return;
    }
    
    if (lua_pcall(m_luaState, 0, 0, 0) != LUA_OK) {
        string error = lua_tostring(m_luaState, -1);
        lua_pop(m_luaState, 1);
        Debug::Logger::LogFatalWithType(ErrorType::ScriptRuntimeError, 
            "Failed to execute script string: {} - {}", scriptName, error);
        return;
    }
}

void Script::GKC_Script::RunScript() {
    GKC_ENGINE_INFO("Running script '{}'...", m_scriptName + ".lua");
    
    lua_getglobal(m_luaState, m_scriptName.c_str());
    
    if (lua_isfunction(m_luaState, -1)) {
        if (lua_pcall(m_luaState, 0, 0, 0) != LUA_OK) {
            string error = lua_tostring(m_luaState, -1);
            lua_pop(m_luaState, 1);
            Debug::Logger::LogFatalWithType(ErrorType::ScriptRuntimeError, 
                "Failed to run script: {} - {}", m_scriptName, error);
            return;
        }
    } else {
        lua_pop(m_luaState, 1);
    }
}

bool Script::CheckScriptExtension(const path& path) {
    if (path.empty() || !Filesystem::CheckFile(path)) {
        return false;
    }
    auto pathStr = path.extension().string();

    if (pathStr == ".lua") { return true; }

    return false;
}