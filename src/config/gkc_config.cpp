#include <config/gkc_config.h>
#include <core/gkc_logger.h>
#include <script/gkc_library.h>

using namespace Galaktic;
using namespace Galaktic::Config;
using namespace Galaktic::Core::Systems;

BindMap BindConfigurator::m_bindMap;

void BindConfigurator::CreateKeyBind(const string& bindName, const Key& key) {
    Bind bind(bindName, key);
    
    if(CheckBindExists(bind)) {
        GKC_ENGINE_INFO("Keybind '{}' already exists!", bindName);
        return;
    }

    m_bindMap.emplace(bindName, bind);
}


void BindConfigurator::CreateMouseBind(const string& bindName, const MouseClick& click)  {
    Bind bind(bindName, click);
    
    if(CheckBindExists(bind)) {
        GKC_ENGINE_INFO("Keybind '{}' already exists!", bindName);
        return;
    }

    m_bindMap.emplace(bindName, bind);
}

void BindConfigurator::DeleteBind(const string& bindName) {
    if(m_bindMap.contains(bindName)) {
        m_bindMap.erase(bindName);
        return;
    }
    GKC_ENGINE_ERROR("Bind '{}' to delete doesn't exist!", bindName);
}

void BindConfigurator::BindAction(const string& functionName, const string& bindName) {
    auto func = Script::LuaGalaktic::GetLuaFunction(functionName);
    if(func == NULL) {
        return;
    }
    if(!CheckBindExists(bindName)) {
        GKC_ENGINE_INFO("Keybind '{}' doesn't exist!", bindName);\
        return;
    }

    auto bind = GetBind(bindName);
    bind.m_callback = func;
}

Bind BindConfigurator::GetBind(const string& bindName) {
    if(CheckBindExists(bindName)) {
        return m_bindMap.find(bindName)->second;
    }
    return m_bindMap.at(0);
}

bool BindConfigurator::CheckBindExists(const Bind& keyBind) {
    auto it = m_bindMap.find(keyBind.m_name);
    if(it != m_bindMap.end()) {
        return true;        // Bind exists
    }
    return false;           // Bind doesn't exists
}

bool BindConfigurator::CheckBindExists(const string& bindName) {
    auto it = m_bindMap.find(bindName);
    if(it != m_bindMap.end()) {
        return true;        // Bind exists
    }
    return false;           // Bind doesn't exists
}

