#include <script/gkc_library.h>
#include <core/managers/gkc_texture_man.h>
#include <core/managers/gkc_audio_man.h>
#include <core/managers/gkc_animation_man.h>
#include <core/helpers/gkc_ecs_helper.h>
#include <core/managers/gkc_script_man.h>
#include <audio/gkc_audio.h>
#include <render/gkc_animation.h>
#include <script/gkc_script.h>
#include <render/gkc_texture.h>
#include <core/systems/gkc_key.h>
#include <core/systems/gkc_mouse_system.h>
#include <filesys/gkc_filesys.h>

using namespace Galaktic::Script;
using namespace Galaktic::Core::Systems;
using namespace Galaktic::Core::Managers;

lua_State* LuaGalaktic::m_luaState = nullptr;
vector<string> LuaGalaktic::m_modulePathList;

bool LuaGalaktic::Initialize() {
    m_luaState = luaL_newstate();
    if (m_luaState == nullptr) {
        return false;
    }
    luaL_openlibs(m_luaState);
    return true;
}

void LuaGalaktic::AddLuaModulePath(const string& path) {
    if(m_luaState == nullptr) {
        GKC_ENGINE_ERROR("Lua state hasn't been initialized!");
        return;
    }

    // Push 'package' table onto Lua's stack, and gets
    // the path variable to add custom modules inside Lua
    lua_getglobal(m_luaState, "package");
    lua_getfield(m_luaState, -1, "path");   
    const char* cstr = lua_tostring(m_luaState, -1);
    string currentPath = (cstr != nullptr) ? cstr : "";
    lua_pop(m_luaState, 1);

    string newPath = currentPath;
    newPath += ";" + path + "/?.lua";
    newPath += ";" + path + "/?/init.lua";

    lua_pushstring(m_luaState, newPath.c_str());
    lua_setfield(m_luaState, -2, "path");
    lua_pop(m_luaState, 1);
}

void LuaGalaktic::SetupModules(const path& scriptRootFolder) {
    string rootPath = scriptRootFolder.string();
    string modulesPath = rootPath + "/modules";
    if (rootPath.empty()) {
        GKC_ENGINE_ERROR("Script root folder path is empty!");
        return;
    }
    

    std::replace(rootPath.begin(), rootPath.end(), '\\', '/');
    
    
    GKC_ENGINE_INFO("Setting up Lua module paths with root: {}", rootPath);

    // Add the folders to the list
    AddModuleFolders(modulesPath);

    // Add the scripts folder and scripts/modules as usable paths for modules
    AddLuaModulePath(rootPath);                           
    AddLuaModulePath(modulesPath);    

    for(auto& folder : m_modulePathList) {
        if(!folder.empty()) {
            AddLuaModulePath(rootPath + "/modules/" + folder);  
            GKC_ENGINE_INFO("Added module '{}'", folder);  
        }
    }        
}

void LuaGalaktic::AddModuleFolders(const path& sriptModulesPath) {
    auto folderPaths = Filesystem::GetFoldersInFolder(sriptModulesPath);
    m_modulePathList = folderPaths;
    if(m_modulePathList.empty()) {
        GKC_ENGINE_WARNING("No modules were found!");
    }
}

void LuaGalaktic::Shutdown() {
    if (m_luaState != nullptr) {
        lua_close(m_luaState);
        m_luaState = nullptr;
    }
}

void LuaGalaktic::BindGalaktic() {
    BindAudioFunctions();
    BindAnimationFunctions();
    BindScriptFunctions();
    BindEntityFunctions();
    BindKeyboardFunctions();
    BindMouseFunctions();
    SetMouseClicksToLua();
    SetKeysToLua();
    BindLoggingFunctions();
}

void LuaGalaktic::BindAudioFunctions() {
    // Galaktic::Audio::AudioManager
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic").beginNamespace("Audio")
        .beginClass<AudioManager>("AudioManager")
            .addStaticFunction("PlayAudioFile", &AudioManager::PlayAudioFile)
            .addStaticFunction("PlayMusicFile", &AudioManager::PlayMusicFile)
            .addStaticFunction("StopSound", &AudioManager::StopSound)
            .addStaticFunction("StopAllTracksFromSound", &AudioManager::StopAllTracksFromSound)
            .addStaticFunction("StopAllSounds", &AudioManager::StopAllSounds)
            .addStaticFunction("PrintList", &AudioManager::PrintList)
        .endClass()
    .endNamespace();
}

void LuaGalaktic::BindAnimationFunctions() {
    // Galaktic::Render::AnimationManager
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic").beginNamespace("Render")
        .beginClass<AnimationManager>("AnimationManager")
            .addStaticFunction("PlayAnimation", &AnimationManager::PlayAnimation)
            .addStaticFunction("AddAnimation", &AnimationManager::PauseAnimation)
            .addStaticFunction("LoadAnimation", &AnimationManager::StopAnimation)
            .addStaticFunction("LoadAllAnimations", &AnimationManager::SetLoopToAnimation)
            .addStaticFunction("PrintList", &AnimationManager::PrintList)
        .endClass()
    .endNamespace();
}

void LuaGalaktic::BindScriptFunctions() {
    // Galaktic::Script::ScriptManager (doesn't match Galaktic namespace for readability purposes)
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic").beginNamespace("Script")
        .beginClass<ScriptManager>("ScriptManager")
            .addStaticFunction("AddInlineScript", &ScriptManager::AddInlineScript)
            .addStaticFunction("AddScriptFromFile", &ScriptManager::AddScriptFromFile)
            .addStaticFunction("RunScript", &ScriptManager::RunScript)
            .addStaticFunction("DeleteScriptFromList", &ScriptManager::DeleteScriptFromList)
        .endClass()
    .endNamespace();
}


///@todo Change this to return the actual entity and add non-static functions for THOSE (Static, Physics, Camera, etc.) objects, add barriers to avoid/
void LuaGalaktic::BindEntityFunctions() {
    // Galaktic::EntityHelper (doesn't match Galaktic namespace for readability purposes)
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginClass<Core::Helpers::ECS_Helper>("EntityHelper")
            .addStaticFunction("CreateStaticObject", &Core::Helpers::ECS_Helper::CreateStaticObject)
            .addStaticFunction("CreatePhysicsObject", &Core::Helpers::ECS_Helper::CreatePhysicsObject)
            .addStaticFunction("CreateLightEntity", &Core::Helpers::ECS_Helper::CreateLightEntity)
            .addStaticFunction("CreateCameraEntity", &Core::Helpers::ECS_Helper::CreateCameraEntity)
            .addStaticFunction("DeleteEntity", &Core::Helpers::ECS_Helper::DeleteEntity)
            .addStaticFunction("AddComponentToEntity", &Core::Helpers::ECS_Helper::AddComponentToEntity)
            .addStaticFunction("RemoveComponentFromEntity", &Core::Helpers::ECS_Helper::RemoveComponentFromEntity)
            .addStaticFunction("GetPlayer", &Core::Helpers::ECS_Helper::GetPlayer)
        .endClass();
}

void LuaGalaktic::BindKeyboardFunctions() {
    // Galaktic::Keyboard
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginClass<Core::Systems::Keyboard>("Keyboard")
            .addStaticFunction("IsKeyDown", &Core::Systems::Keyboard::IsKeyDownLua)
        .endClass()
    .endNamespace();
}

void LuaGalaktic::BindMouseFunctions() {
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginClass<Core::Systems::Mouse>("Mouse")
            .addStaticFunction("IsMouseDown", &Core::Systems::Mouse::IsMouseDown)
        .endClass()
    .endNamespace();
}

#define BIND_KEY(enumName) .addConstant(#enumName, (uint32_t)Key::enumName)

void LuaGalaktic::SetKeysToLua() {
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginNamespace("Keys")
            BIND_KEY(Unknown)
            BIND_KEY(Return)
            BIND_KEY(Escape)
            BIND_KEY(Backspace)
            BIND_KEY(Tab)
            BIND_KEY(Space)
            
            BIND_KEY(Key0)
            BIND_KEY(Key1)
            BIND_KEY(Key2)
            BIND_KEY(Key3)
            BIND_KEY(Key4)
            BIND_KEY(Key5)
            BIND_KEY(Key6)
            BIND_KEY(Key7)
            BIND_KEY(Key8)
            BIND_KEY(Key9)
            
            BIND_KEY(A)
            BIND_KEY(B)
            BIND_KEY(C)
            BIND_KEY(D)
            BIND_KEY(E)
            BIND_KEY(F)
            BIND_KEY(G)
            BIND_KEY(H)
            BIND_KEY(I)
            BIND_KEY(J)
            BIND_KEY(K)
            BIND_KEY(L)
            BIND_KEY(M)
            BIND_KEY(N)
            BIND_KEY(O)
            BIND_KEY(P)
            BIND_KEY(Q)
            BIND_KEY(R)
            BIND_KEY(S)
            BIND_KEY(T)
            BIND_KEY(U)
            BIND_KEY(V)
            BIND_KEY(W)
            BIND_KEY(X)
            BIND_KEY(Y)
            BIND_KEY(Z)
            
            BIND_KEY(F1)
            BIND_KEY(F2)
            BIND_KEY(F3)
            BIND_KEY(F4)
            BIND_KEY(F5)
            BIND_KEY(F6)
            BIND_KEY(F7)
            BIND_KEY(F8)
            BIND_KEY(F9)
            BIND_KEY(F10)
            BIND_KEY(F11)
            BIND_KEY(F12)
            
            BIND_KEY(Left)
            BIND_KEY(Right)
            BIND_KEY(Up)
            BIND_KEY(Down)
            
            BIND_KEY(LeftShift)
            BIND_KEY(RightShift)
            BIND_KEY(LeftCtrl)
            BIND_KEY(RightCtrl)
            BIND_KEY(LeftAlt)
            BIND_KEY(RightAlt)
            BIND_KEY(LeftMeta)
            BIND_KEY(RightMeta)
            
            BIND_KEY(Insert)
            BIND_KEY(Delete)
            BIND_KEY(Home)
            BIND_KEY(End)
            BIND_KEY(PageUp)
            BIND_KEY(PageDown)
            
            BIND_KEY(KeyPad0)
            BIND_KEY(KeyPad1)
            BIND_KEY(KeyPad2)
            BIND_KEY(KeyPad3)
            BIND_KEY(KeyPad4)
            BIND_KEY(KeyPad5)
            BIND_KEY(KeyPad6)
            BIND_KEY(KeyPad7)
            BIND_KEY(KeyPad8)
            BIND_KEY(KeyPad9)
            BIND_KEY(KeyPadPlus)
            BIND_KEY(KeyPadMinus)
            BIND_KEY(KeyPadMultiply)
            BIND_KEY(KeyPadDivide)
            BIND_KEY(KeyPadEnter)
            BIND_KEY(KeyPadDecimal)
        .endNamespace()
    .endNamespace();
}

#undef BIND_KEY

#define BIND_CLICK(enumName) .addConstant(#enumName, (uint32_t) MouseClick::enumName)

void LuaGalaktic::SetMouseClicksToLua() {
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginNamespace("MouseClicks")
            BIND_CLICK(LeftClick)
            BIND_CLICK(RightClick)
            BIND_CLICK(MiddleClick)
        .endNamespace()
    .endNamespace();
}

#undef BIND_CLICK


void LuaGalaktic::BindLoggingFunctions() {
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginNamespace("Logger")
            .addFunction("LogInfo", &Galaktic::Debug::Logger::LogInfoLua)
            .addFunction("LogWarning", &Galaktic::Debug::Logger::LogWarningLua)
            .addFunction("LogError", &Galaktic::Debug::Logger::LogErrorLua)
        .endNamespace()
    .endNamespace();
}

string LuaGalaktic::GetLuaCallContext(int level) {
    lua_Debug ar;
    if (lua_getstack(m_luaState, level, &ar) && lua_getinfo(m_luaState, "nSl", &ar)) {
        string source = ar.source ? ar.source : "?";
        if (source[0] == '@') source = source.substr(1);
        
        size_t lastSlash = source.find_last_of("/\\");
        if (lastSlash != string::npos) {
            source = source.substr(lastSlash + 1);
        }
        
        string funcName = ar.name ? ar.name : "chunk";
        int line = ar.currentline > 0 ? ar.currentline : 0;
        
        return "[" + source + " -> " + funcName + " || Line: " + 
               to_string(line) + "]";
    }
    return "[? -> ? || Line: 0]";
}

//@TODO end this plz
void LuaGalaktic::CreateDefaultUpdateFile(const path& path) {
    if(Filesystem::CheckFile(path / "Player.lua"))
        return;
    
    ofstream file(path / "Player.lua");
    GKC_ENSURE_FILE_OPEN(file, WritingError);

    const string PLAYER_LUA_CONTENT = 
        "-- Player.lua"
        "local gkc = require(\"Galaktic\")"
        "Player = gkc.EntityHelper.GetPlayer()"
        "function Movement(dt)"
        "   "
        "end";

    file << PLAYER_LUA_CONTENT;
    file.close();
}

void LuaGalaktic::CreateDefaultPlayerFile(const path& path) {

}

void LuaGalaktic::PrintLuaStack()
{
    lua_State* L = m_luaState; 

    int top = lua_gettop(L); 

    if (top == 0) 
    {
        GKC_ENGINE_INFO("Lua stack is empty!");
        return;
    }

    GKC_ENGINE_INFO( "[Lua Stack] ({} elements)", top);

    for (int i = top; i >= 1; i--) 
    {
        int type = lua_type(L, i);
        GKC_ENGINE_INFO("[{}] : " , lua_typename(L, type));

        switch (type) {
            case LUA_TSTRING:
                GKC_ENGINE_INFO(lua_tostring(L, i));
                break;
            case LUA_TNUMBER:
                GKC_ENGINE_INFO(lua_tonumber(L, i));
                break;
            case LUA_TBOOLEAN:
                GKC_ENGINE_INFO(lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNIL:
                GKC_ENGINE_INFO("nil");
                break;
            case LUA_TTABLE:
            case LUA_TFUNCTION:
            case LUA_TUSERDATA:
            case LUA_TLIGHTUSERDATA:
            case LUA_TTHREAD:
                GKC_ENGINE_INFO(lua_topointer(L, i));
                break;
            default:
                GKC_ENGINE_INFO("? type");
                break;
        }
    }
}