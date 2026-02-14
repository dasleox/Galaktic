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

void LuaGalaktic::AddLuaModulePath(const path& path) {
    if(m_luaState == nullptr) {
        GKC_ENGINE_ERROR("Lua state hasn't been initialized!");
        return;
    }

    lua_getglobal(m_luaState, "package");
    lua_getfield(m_luaState, -1, "path");
    string currentPath = lua_tostring(m_luaState, -1);
    lua_pop(m_luaState, -1);

    string newPath = lua_tostring(m_luaState, -1);
    newPath += ";" + path.string()  += "/?.lua";
    newPath += ";" + path.string() + "/?/init.lua";

    lua_pushstring(m_luaState, newPath.c_str());
    lua_setfield(m_luaState, -2, "path");
    lua_pop(m_luaState, 1);
}

void LuaGalaktic::SetupModules(const path& scriptRootFolder) {
    string rootPath = scriptRootFolder.string();
    string modulesPath = rootPath + "/modules";
    
    std::replace(rootPath.begin(), rootPath.end(), '\\', '/');
    
    GKC_ENGINE_INFO("Setting up Lua module paths with root: {}", rootPath);

    AddModuleFolders(modulesPath);
    AddLuaModulePath(rootPath);                           
    AddLuaModulePath(modulesPath);    

    for(auto& folder : m_modulePathList) {
        AddLuaModulePath(rootPath + "/modules/" + folder);     
    }        
}

void LuaGalaktic::AddModuleFolders(const path& sriptModulesPath) {
    auto folderPaths = Filesystem::GetFoldersInFolder(sriptModulesPath);
    m_modulePathList = folderPaths;
}

bool LuaGalaktic::CallLuaFunction(const string &functionName) {
    auto func = luabridge::getGlobal(m_luaState, functionName.c_str());
    if(func.isFunction()) {
        func();
        return true;
    } else {
        GKC_ENGINE_ERROR("Function '{}' doesn't exist or is not a function!", functionName);
        return false;
    }
}

void LuaGalaktic::Shutdown() {
    // CRITICAL FIX: Close the Lua state to free all memory
    if (m_luaState != nullptr) {
        lua_close(m_luaState);
        m_luaState = nullptr;
        GKC_ENGINE_INFO("Lua state closed successfully!");
    }
}

void LuaGalaktic::BindGalaktic() {
    BindAudioFunctions();
    BindTextureFunctions();
    BindAnimationFunctions();
    BindScriptFunctions();
    BindEntityFunctions();
    BindKeyboardFunctions();
    BindMouseFunctions();
    SetMouseClicksToLua();
    SetKeysToLua();
}

void LuaGalaktic::BindTextureFunctions() {
    // Galaktic::Render::TextureManager
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic").beginNamespace("Render")
        .beginClass<TextureManager>("TextureManager")
            .addStaticFunction("AddTexture", &TextureManager::AddTexture)
            .addStaticFunction("AddTexturePath", &TextureManager::AddTexturePath)
            .addStaticFunction("LoadTexture", &TextureManager::LoadTexture)
            .addStaticFunction("LoadAllTextures", &TextureManager::LoadAllTextures)
            .addStaticFunction("DeleteTexture", &TextureManager::DeleteTexture)
            .addStaticFunction("PrintList", &TextureManager::PrintList)
        .endClass()
    .endNamespace();
}

void LuaGalaktic::BindAudioFunctions() {
    // Galaktic::Audio::AudioManager
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic").beginNamespace("Audio")
        .beginClass<AudioManager>("AudioManager")
            .addStaticFunction("AddAudioFile", &AudioManager::AddAudioFile)
            .addStaticFunction("RemoveAudioFile", &AudioManager::RemoveAudioFile)
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
            .addStaticFunction("AddAnimationPath", &AnimationManager::AddAnimationPath)
            .addStaticFunction("AddAnimation", &AnimationManager::AddAnimation)
            .addStaticFunction("LoadAnimation", &AnimationManager::LoadAnimation)
            .addStaticFunction("LoadAllAnimations", &AnimationManager::LoadAllAnimations)
            .addStaticFunction("DeleteAnimation", &AnimationManager::DeleteAnimation)
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


///@todo Change this to return the actual entity and add non-static functions for THOSE (Static, Physics, Camera, etc.) objects, add barriers to avoid
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
        .endClass();
}

void LuaGalaktic::BindKeyboardFunctions() {
    // Galaktic::Keyboard
    luabridge::getGlobalNamespace(m_luaState).beginNamespace("Galaktic")
        .beginClass<Core::Systems::Keyboard>("Keyboard")
            .addStaticFunction("IsKeyDown", &Core::Systems::Keyboard::IsKeyDown)
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