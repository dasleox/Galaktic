#include <core/managers/gkc_script_man.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>
#include <script/gkc_script.h>

using namespace Galaktic;
using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;

Script::Script_List ScriptManager::m_scriptList;
unordered_map<ScriptID, string> ScriptManager::m_IDToNameList;
lua_State* ScriptManager::m_luaState = nullptr;

ScriptManager::ScriptManager(const string& folder, lua_State* luaState) {
    m_luaState = luaState;
    auto files = Filesystem::GetFilenamesInFolder(folder + "/local");
    for (auto& file : files) {
        if (Script::CheckScriptExtension(file))
            AddScriptFromFile(file);
    }

    //ExecuteGalakticModule();
}

void ScriptManager::AddInlineScript(const string& scriptName, const string& script) {
    using namespace Script;
    ScriptID id = m_scriptList.size() + 1;
    auto scriptInfo = ScriptInfo(id, make_shared<Script::GKC_Script>(scriptName, script, m_luaState), ScriptType::InlineScript);

    m_scriptList.emplace(scriptName, scriptInfo);
    m_IDToNameList.emplace(id, scriptName);
    GKC_ENGINE_INFO("Added inline script '{0}' (ID: {1})", scriptName, id );
}

void ScriptManager::AddScriptFromFile(const string& scriptPath) {
    using namespace Script;
    ScriptID id = m_scriptList.size() + 1;
    auto scriptInfo = ScriptInfo(id, make_shared<Script::GKC_Script>(scriptPath, m_luaState), ScriptType::FileScript);
    string scriptName = Filesystem::GetFilename(scriptPath);

    m_scriptList.emplace(scriptName, scriptInfo);
    m_IDToNameList.emplace(id, scriptName);
    GKC_ENGINE_INFO("Added file script '{0}' (ID: {1})", scriptName, id );
}

void ScriptManager::DeleteScriptFromList(const string& scriptName) {
    auto it = m_scriptList.find(scriptName);
    if (it != m_scriptList.end()) {
        m_IDToNameList.erase(it->second.scriptID_);
        m_scriptList.erase(scriptName);
        GKC_ENGINE_INFO("Deleted script '{}'", scriptName);
    }
}

shared_ptr<Script::GKC_Script> ScriptManager::GetScriptFromName(const string& scriptName) {
    auto it = m_scriptList.find(scriptName);
    if(it != m_scriptList.end()) {
        return it->second.script_;
    }
    GKC_ENGINE_WARNING("Script to retrieve not found!");
    return nullptr;
}

shared_ptr<Script::GKC_Script> ScriptManager::GetScriptFromID(ScriptID id) {
    auto it = m_IDToNameList.find(id);
    if (it != m_IDToNameList.end()) {
        auto scriptNameIt = m_scriptList.find(it->second);
        if (scriptNameIt != m_scriptList.end()){
            return scriptNameIt->second.script_;
        }
    }
    GKC_ENGINE_WARNING("Script to retrieve not found!");
    return nullptr;
}

void ScriptManager::RunScript(const string &scriptName) {
    auto script = GetScriptFromName(scriptName);
    if (script) {
        script->RunScript();
    } else {
        GKC_ENGINE_ERROR("Script '{}' not found!", scriptName);
    }
}

void ScriptManager::ExecuteGalakticModule() {
    auto galakticFile = m_scriptList.find("Galaktic.lua");
    auto gkcConstantsFile = m_scriptList.find("GlobalConstants.lua");

    galakticFile->second.script_->RunScript();
    gkcConstantsFile->second.script_->RunScript();
}