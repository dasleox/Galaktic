#include <core/managers/gkc_script_man.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>

using namespace Galaktic;
using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;

Script::Script_List ScriptManager::m_scriptList;
std::multimap<ScriptID, string> ScriptManager::m_IDToNameList;

ScriptManager::ScriptManager(const path& folder) {
    auto files = Filesystem::GetFilenamesInFolder(folder);
    for (auto& file : files) {
        if (Script::CheckScriptExtension(file))
            AddScriptFromFile(file);
    }
}

void ScriptManager::AddInlineScript(const string& scriptName, const string& script) {
    using namespace Script;
    ScriptID id = m_scriptList.size() + 1;
    auto scriptInfo = ScriptInfo(id, GKC_Script(scriptName, script), ScriptType::InlineScript);

    m_scriptList.emplace(scriptName, scriptInfo);
    m_IDToNameList.emplace(id, scriptName);
    GKC_ENGINE_INFO("Added inline script '{0}' (ID: {1})", scriptName, id );
}

void ScriptManager::AddScriptFromFile(const path& scriptPath) {
    using namespace Script;
    ScriptID id = m_scriptList.size() + 1;
    auto scriptInfo = ScriptInfo(id, GKC_Script(scriptPath), ScriptType::FileScript);
    string scriptName = scriptPath.filename().string();

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

Script::GKC_Script* ScriptManager::GetScriptFromName(const string& scriptName) {
    auto it = m_scriptList.find(scriptName);
    if(it != m_scriptList.end()) {
        return &it->second.script_;
    }
    GKC_ENGINE_WARNING("Script not found!");
    return nullptr;
}

Script::GKC_Script* Galaktic::Core::Managers::ScriptManager::GetScriptFromID(ScriptID id) {
    auto it = m_IDToNameList.find(id);
    if (it != m_IDToNameList.end()) {
        auto scriptNameIt = m_scriptList.find(it->second);
        if (scriptNameIt != m_scriptList.end()){
            return &scriptNameIt->second.script_;
        }
    }
    GKC_ENGINE_WARNING("Script not found!");
    return nullptr;
}