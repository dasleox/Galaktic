#include <script/gkc_script.h>
#include <core/gkc_exception.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>

using namespace Galaktic;

Script::GKC_Script::GKC_Script(const path& scriptPath, lua_State* luaState) 
    : m_scriptName(Core::StripExtension(scriptPath.filename().string())) {

    m_luaState = luaState;
    try {
        if (luaL_loadfile(m_luaState, scriptPath.string().c_str()) != LUA_OK) {
            string error = lua_tostring(m_luaState, -1);
            lua_pop(m_luaState, 1);
            GKC_THROW_EXCEPTION(Debug::ScriptException, "Failed to compile script file: " + scriptPath.string() + " - " + error);
        }

        if (lua_pcall(m_luaState, 0, 0, 0) != LUA_OK) {
            string error = lua_tostring(m_luaState, -1);
            lua_pop(m_luaState, 1);
            GKC_THROW_EXCEPTION(Debug::ScriptException, "Failed to execute script file: " + scriptPath.string() + " - " + error);
        }
    } catch (const Debug::ScriptException& e) {
        throw;
    }
}

Script::GKC_Script::GKC_Script(const string& scriptName, const string& scriptString, lua_State* luaState) 
    : m_scriptName(scriptName) {
    m_luaState = luaState;
    try {
        if (luaL_loadbuffer(m_luaState, scriptString.c_str(), scriptString.size(), 
                           scriptName.c_str()) != LUA_OK) {
            string error = lua_tostring(m_luaState, -1);
            lua_pop(m_luaState, 1);
            GKC_THROW_EXCEPTION(Debug::ScriptException, "Failed to compile script string: " + scriptName + " - " + error);
        }
        
        if (lua_pcall(m_luaState, 0, 0, 0) != LUA_OK) {
            string error = lua_tostring(m_luaState, -1);
            lua_pop(m_luaState, 1);
            GKC_THROW_EXCEPTION(Debug::ScriptException, "Failed to execute script string: " + scriptName + " - " + error);
        }
    } catch (const Debug::ScriptException& e) {
        throw;
    }
}

void Script::GKC_Script::RunScript() {
    GKC_ENGINE_INFO("Running script '{}'...", m_scriptName + ".lua");
    
    lua_getglobal(m_luaState, m_scriptName.c_str());
    
    if (lua_isfunction(m_luaState, -1)) {
        if (lua_pcall(m_luaState, 0, 0, 0) != LUA_OK) {
            string error = lua_tostring(m_luaState, -1);
            lua_pop(m_luaState, 1);
            GKC_THROW_EXCEPTION(Debug::ScriptException, 
                "Failed to run script: " + m_scriptName + " - " + error);
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