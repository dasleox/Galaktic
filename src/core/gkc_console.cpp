#include <core/gkc_console.h>
#include <core/managers/gkc_ecs_man.h>
#include <core/gkc_logger.h>
#include <core/managers/gkc_script_man.h>
#include <core/managers/gkc_scene_man.h>
#include <core/managers/gkc_audio_man.h>
#include <config/gkc_config.h>
#include <script/gkc_library.h>
#include <core/gkc_scene.h>

using namespace Galaktic;
using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;

#define SET_ARGUMENT_QUANTITY(quantity, args)                               \
    do {                                                                    \
        if(!(args.size()) == (quantity)) {                                  \
            string errorMsg = "Given " + to_string(args.size()) +           \
                " of " + to_string(quantity) + " arguments!";               \
            GKC_ENGINE_ERROR(errorMsg);                                     \
            return;                                                         \
        }                                                                   \
    } while(0)
    
Console::Console(shared_ptr<Debug::ConsoleMutex_mt> sink)
{
    m_sink = sink;
}

void Console::Display(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight) 
{
    if(!m_isVisible)
    {
        return;
    } 

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0 , 200);
    SDL_FRect consolePanel = {0, 0, (float) windowWidth, (float) windowHeight * 0.4f};
    SDL_RenderFillRect(renderer, &consolePanel);

    auto& lines = m_sink->GetLines();
    int lineHeight  = TTF_GetFontHeight(font) + 2;
    int maxLines = (int)(consolePanel.h / lineHeight);
    int startIdx = std::max(0, (int)lines.size() - maxLines);

    float y = consolePanel.h - (float)lineHeight;

    for (int i = (int)lines.size() - 1; i >= startIdx; i--, y -= lineHeight) {
        SDL_Color color = LevelToColor(lines[i].m_level);
        SDL_Surface* surf = TTF_RenderText_Blended(font, lines[i].m_text.c_str(), 0, color);
        if (!surf) continue;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FRect dst = { 4.f, y, (float)surf->w, (float)surf->h };
        SDL_RenderTexture(renderer, tex, NULL, &dst);
        SDL_DestroySurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void Console::GetSceneContext(Scene* scene) 
{
    GKC_RELEASE_ASSERT(scene != nullptr, "Current context is NULL!, no scene exists!");
    m_currentScene = scene;
}

void Console::ShowDebugInformation()
{

}

void Console::ShowFPS()
{

}

void Console::DisplayHelp()
{

}

void Console::ClearConsole()
{

}

void Console::ExecuteCommand(const string& command) 
{
    size_t indexOfSpace = command.find_first_of(" ");
    string literalCommand = command.substr(0, indexOfSpace);  
    string arguments = command.substr(indexOfSpace, command.size());
    arguments = arguments.substr(0, arguments.size());
    
    if(indexOfSpace == string::npos && arguments.empty()) 
    {
        auto wrapper = NoArgumentCommands(command);
        CommandSwitch(wrapper);
        return;
    }
    else if (indexOfSpace == string::npos && !arguments.empty()) 
    {
        auto wrapper = ArgumentCommands(command, arguments);
        CommandSwitch(wrapper);
        return;
    }
}

void Console::CommandSwitch(const CommandWrapper& wrapper)
{
    SceneCommands(wrapper);
    EntityCommands(wrapper);
    ScriptCommands(wrapper);
    AppCommands(wrapper);
    SoundCommands(wrapper);
    DebugCommands(wrapper);
}

//@todo Check for the string of each one

#define INVALID_CONVERION_ERROR(type) GKC_ENGINE_ERROR("failed to convert argument to '{}' type", #type)

void Console::SceneCommands(const CommandWrapper& wrapper) 
{
    const auto& manager = m_currentScene->GetECSManager();
    const vector<string> args = GetListOfArguments(wrapper.args);

    switch(wrapper.type)
    {
        case CommandTypes::LoadScene: 
        {
            if(m_sceneManager->SceneExists(args[0]))
            {
                InvalidArgumentError("file doesn't exists!");
                return;
            }
            m_sceneManager->LoadSpecificSceneFromFile(args[0]);
            break;
        }
        case CommandTypes::SaveCurrentScene: 
        {
            m_currentScene->Save();
            break;
        }
        case CommandTypes::ShowSceneList:
        {
            m_sceneManager->ShowSceneList();
            break;
        }
        case CommandTypes::PauseToggleScene:
        {
            m_currentScene->Pause();
            break;
        }
    }
}

void Console::EntityCommands(const CommandWrapper& wrapper)
{
    const auto& manager = m_currentScene->GetECSManager();
    const vector<string> args = GetListOfArguments(wrapper.args);
    
    switch(wrapper.type)
    {
        case CommandTypes::CreateStaticObject: 
        {
            manager->CreateEntity<ECS::StaticObjectTag>(args[0]);
            break;
        }
        case CommandTypes::CreatePhysicsObject:
        {
            manager->CreateEntity<ECS::PhysicsObjectTag>(args[0]);
            break;
        }
        case CommandTypes::DeleteEntity:
        {
            manager->DeleteEntityByName(args[0]);
            break;
        }
        case CommandTypes::TeleportEntity:
        {
            SET_ARGUMENT_QUANTITY(3, args);
            try 
            {
                float x = std::stof(args[1]);
                float y = std::stof(args[2]);
                manager->TeleportEntityByName(args[0],x, y);
            } 
            catch(const std::invalid_argument&) 
            {   
                INVALID_CONVERION_ERROR(float);
            }
            catch(const std::out_of_range&)
            {
                GKC_ENGINE_ERROR("coordinate out of range!");
            }
            break;
        }
        case CommandTypes::SetRotationEntity:
        {
            SET_ARGUMENT_QUANTITY(2, args);
            try 
            {
                float degrees = std::stof(args[1]);
                manager->RotateEntityByName(args[0], degrees);
            } 
            catch(const std::invalid_argument&) 
            {   
                INVALID_CONVERION_ERROR(float);
            }
            break;
        }
        case CommandTypes::DisplayEntityInformation:
        {
            manager->DisplayEntityInformation(args[0]);
            break;
        }
        case CommandTypes::PrintEntityList:
        {
            manager->PrintEntityList();
            break;
        }
    }
}

void Console::ScriptCommands(const CommandWrapper& wrapper) 
{
    const auto& manager = m_currentScene->GetECSManager();
    const vector<string> args = GetListOfArguments(wrapper.args);

    switch(wrapper.type)
    {
        case CommandTypes::ExecuteScript:
        {
            ScriptManager::RunScript(args[0]);
            break;
        }
        case CommandTypes::PrintLuaStack:
        {
            Script::LuaGalaktic::PrintLuaStack();
            break;
        }
        case CommandTypes::BindAction:
        {
            Config::BindConfigurator::BindAction(args[0], args[1]); 
            break;
        }
    }
}

void Console::AppCommands(const CommandWrapper& wrapper)
{
    const string& args = wrapper.args;
    switch(wrapper.type)
    {
        case CommandTypes::FullscreenToggle: 
        {
            m_currentScene->SetFullscreen();
            break;
        }
        case CommandTypes::CheatsToggle:
        {
            bool toggle = false;
            if(args == "true") toggle = true;
            else if(args == "false") toggle = false;
            else { 
                InvalidArgumentError("Argument should be true or false!");
            }
    
            m_cheatsToggle = toggle;
            break;
        }
        case CommandTypes::ClearConsole:
        {
            ClearConsole();
            break;
        }
        case CommandTypes::HelpConsole: 
        {
            DisplayHelp();
            break;
        }
    }
}

void Console::SoundCommands(const CommandWrapper& wrapper)
{

    switch(wrapper.type)
    {
        case CommandTypes::PlaySound: {
            //m_currentScene->m_audioManager->PlayAudioFile(wrapper.args);
            break;
        }
        case CommandTypes::StopSound: {
           // m_currentScene->m_audioManager->StopSound(wrapper.args);
           break;
        }
        case CommandTypes::StopAllSounds: {
            // m_currentScene->m_audioManager->StopAllSounds();
            break;
        }
        case CommandTypes::PrintSoundList: {
            // m_currentScene->m_audioManager->PrintList();
            break;
        }
    }
}

void Console::DebugCommands(const CommandWrapper& wrapper) 
{
    auto& manager = m_currentScene->GetECSManager();

    switch(wrapper.type)
    {
        case CommandTypes::ShowFPSToggle:
        {
            ShowFPS();
            break;
        }
        case CommandTypes::ShowWireframeToggle:
        {
            m_currentScene->ToggleWireframes();
            break;
        }
        case CommandTypes::ShowCollidersToggle:
        {
            m_currentScene->ToggleColliders();
            break;
        }
        case CommandTypes::ShowDebugInformationToggle:
        {
            ShowDebugInformation();
            break;
        }
    }
}

void Console::InvalidArgumentError(const string& message)
{
    GKC_ENGINE_ERROR("Invalid Argument: '{}'" , message);
}

vector<string> Console::GetListOfArguments(const string& args) {
    vector<string> arguments;
    string accumulator;

    for(auto& c : args) 
    {
        if(c == 'c')
        {
            arguments.push_back(accumulator);
            accumulator.clear();
        }
        accumulator += c;
    }   

    return arguments;
}

#define COM_CHECK(str) ((command) == (str))
#define COM_WRAPPER_RETURN(str, paramType)                 \
    do {                                              \
        if(COM_CHECK(str)) {                          \
            wrapper.type = (paramType);                  \
            return (wrapper);                         \
        }                                             \
    } while(0)

    
CommandWrapper Console::NoArgumentCommands(const string& command) 
{
    auto wrapper = CommandWrapper(command, "", CommandTypes::Unknown);

    COM_WRAPPER_RETURN("save", CommandTypes::SaveCurrentScene);
    COM_WRAPPER_RETURN("pause", CommandTypes::PauseToggleScene);
    COM_WRAPPER_RETURN("show_scene_list", CommandTypes::ShowSceneList);
    COM_WRAPPER_RETURN("print_entity_list", CommandTypes::PrintEntityList);
    COM_WRAPPER_RETURN("print_luastack", CommandTypes::PrintLuaStack);
    COM_WRAPPER_RETURN("help", CommandTypes::HelpConsole);
    COM_WRAPPER_RETURN("clear", CommandTypes::ClearConsole);
    COM_WRAPPER_RETURN("print_sound_list", CommandTypes::PrintSoundList);

    return CommandWrapper{};
}

CommandWrapper Console::ArgumentCommands(const string& command, const string& args)
{
    if(args.empty()) 
    {
        return NoArgumentCommands(command);
    }

    auto wrapper = CommandWrapper(command, args, CommandTypes::Unknown);

    COM_WRAPPER_RETURN("load", CommandTypes::LoadScene);
    COM_WRAPPER_RETURN("create_static_object", CommandTypes::CreateStaticObject);
    COM_WRAPPER_RETURN("create_physics_object", CommandTypes::CreatePhysicsObject);
    COM_WRAPPER_RETURN("delete_entity", CommandTypes::DeleteEntity);
    COM_WRAPPER_RETURN("teleport_entity", CommandTypes::TeleportEntity);
    COM_WRAPPER_RETURN("set_rotation_entity", CommandTypes::SetRotationEntity);
    COM_WRAPPER_RETURN("get_entity_info", CommandTypes::DisplayEntityInformation);
    COM_WRAPPER_RETURN("execute_script", CommandTypes::ExecuteScript);
    COM_WRAPPER_RETURN("bind", CommandTypes::BindAction);
    COM_WRAPPER_RETURN("set_fullscreen", CommandTypes::FullscreenToggle);
    COM_WRAPPER_RETURN("set_cheats", CommandTypes::CheatsToggle);
    COM_WRAPPER_RETURN("play_sound", CommandTypes::PlaySound);
    COM_WRAPPER_RETURN("stop_sound", CommandTypes::StopSound);
    COM_WRAPPER_RETURN("stop_all_sounds", CommandTypes::StopAllSounds);
    COM_WRAPPER_RETURN("show_fps", CommandTypes::ShowFPSToggle);
    COM_WRAPPER_RETURN("show_wireframe", CommandTypes::ShowWireframeToggle);
    COM_WRAPPER_RETURN("show_colliders", CommandTypes::ShowCollidersToggle);
    COM_WRAPPER_RETURN("show_debug_info", CommandTypes::ShowDebugInformationToggle);

    return CommandWrapper{};
}

SDL_Color Console::LevelToColor(spdlog::level::level_enum level)
{
     switch (level) {
        case spdlog::level::warn:     return { 255, 255,   0, 255 }; // yellow
        case spdlog::level::err:      return { 255,   0,   0, 255 }; // red
        case spdlog::level::critical: return { 255,   0, 255, 255 }; // magenta
        default:                      return { 0, 0, 255, 255 }; // white
    }
}

#undef COM_CHECK
#undef COM_WRAPPER_RETURN
#undef SET_ARGUMENT_QUANTITY
#undef INVALID_CONVERION_ERROR
