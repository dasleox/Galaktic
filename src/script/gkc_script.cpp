#include <script/gkc_script.h>
#include <core/gkc_exception.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>

using namespace Galaktic;

Script::GKC_Script::GKC_Script(const path& scriptPath) 
    : m_scriptName(Core::StripExtension(scriptPath.filename().string()))
{
    try {
        m_context.CompileFile(m_scriptName, scriptPath.string());
    } catch (const Debug::ScriptException& e) {
        GKC_THROW_EXCEPTION(Debug::ScriptException, "Failed to compile script file: " + scriptPath.string());
    }
}

Script::GKC_Script::GKC_Script(const string& scriptName, const string& scriptString) {
    try {
        m_context.CompileString(scriptName, scriptString);
    }
    catch(const Debug::ScriptException& e) {
        GKC_THROW_EXCEPTION(Debug::ScriptException, "Failed to compile script file!: " + scriptName);
    }
    
}

void Script::GKC_Script::RunScript() {
    GKC_ENGINE_INFO("Running script '{}'...", m_scriptName + ".lua");
    m_context.Run(m_scriptName);
}

bool Script::CheckScriptExtension(const path& path) {
    if (path.empty() || !Filesystem::CheckFile(path)) {
        return false;
    }
    auto pathStr = path.extension().string();

    if (pathStr == ".lua") { return true; }

    return false;
}