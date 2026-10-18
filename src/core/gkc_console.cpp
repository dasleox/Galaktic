#include <core/gkc_console.h>
#include <core/managers/gkc_ecs_man.h>
#include <core/gkc_logger.h>
#include <core/managers/gkc_script_man.h>
#include <core/managers/gkc_scene_man.h>
#include <core/managers/gkc_audio_man.h>
#include <config/gkc_config.h>
#include <script/gkc_library.h>
#include <core/gkc_scene.h>
#include <core/gkc_clock.h>
#include <core/gkc_debugger.h>
#include <core/events/gkc_dispatcher.h>
#include <core/systems/gkc_key.h>
#include <sstream>

using namespace Galaktic;
using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;

/*
 * Sets the argument quantity for command validation.
 * @note The validation is only need when more or equal than 2 arguments are passed
 */
#define SET_ARGUMENT_QUANTITY(quantity, args)                               \
    do {                                                                    \
        if((args).size() != (quantity)) {                                   \
            string errorMsg = "Given " + to_string((args).size()) +         \
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_FRect consolePanel = {0, 0, (float) windowWidth, (float) windowHeight * 0.4f};
    SDL_RenderFillRect(renderer, &consolePanel);

    // The panel still shows without a font so toggling the console is
    // visibly doing something even if text rendering isn't available.
    if (font == nullptr)
    {
        return;
    }

    int lineHeight = TTF_GetFontHeight(font) + 2;

    // Bottom row of the panel is reserved for the input line; log lines
    // stack upward from just above it, most recent line closest to input.
    float inputRowY = consolePanel.h - (float)lineHeight;

    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderLine(renderer, 0.f, inputRowY, consolePanel.w, inputRowY);

    auto& lines = m_sink->GetLines();
    int maxLines = std::max(0, (int)((consolePanel.h - lineHeight) / lineHeight));
    int startIdx = std::max(0, (int)lines.size() - maxLines);

    float y = inputRowY - (float)lineHeight;

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

    string promptLine = "> " + m_inputBuffer;
    SDL_Surface* promptSurf = TTF_RenderText_Blended(font, promptLine.c_str(), 0, WHITE_COLOR);
    if (promptSurf != nullptr) {
        SDL_Texture* promptTex = SDL_CreateTextureFromSurface(renderer, promptSurf);
        SDL_FRect promptDst = { 4.f, inputRowY + 1.f, (float)promptSurf->w, (float)promptSurf->h };
        SDL_RenderTexture(renderer, promptTex, NULL, &promptDst);
        SDL_DestroySurface(promptSurf);
        SDL_DestroyTexture(promptTex);
    }
}

void Console::GetSceneContext(Scene* scene) 
{
    GKC_RELEASE_ASSERT(scene != nullptr, "Current context is NULL!, no scene exists!");
    m_currentScene = scene;
}

void Console::OnEvent(Events::GKC_Event& e)
{
    static std::stack<string> tempStack = std::stack<string>();

    using namespace Events;
    GKC_EventDispatcher dispatcher(e);

    dispatcher.DispatchEvent<TextInputEvent>(
        [this](TextInputEvent& textEvent) {
            if (!m_isVisible)
                return false;
            m_inputBuffer += textEvent.GetText();
            return true;
        });

    dispatcher.DispatchEvent<KeyPressedEvent>(
        [this](KeyPressedEvent& keyEvent) {
            Systems::Key key = GKC_SDLK_TO_ENUM(keyEvent);
            if (key == Systems::Key::F3)
                return false;

            if (!m_isVisible)
                return false;

            if (key == Systems::Key::Backspace) {
                if (!m_inputBuffer.empty()) 
                    m_inputBuffer.pop_back();
            } 
            else if (key == Systems::Key::Return) {
                if (!m_inputBuffer.empty()) {
                    GKC_ENGINE_INFO("> {}", m_inputBuffer);
                    ExecuteCommand(m_inputBuffer);
                    m_commandHistory.push(m_inputBuffer);
                    tempStack = m_commandHistory;
                    m_inputBuffer.clear();
                }
            }

            // Up arrow key retrieves the last command from history, if available.
            if(key == Systems::Key::Up) {
                if(!tempStack.empty()) {
                    m_inputBuffer = tempStack.top();
                    tempStack.pop();
                }
            }

            return true;
        });
}

void Console::ShowDebugInformation()
{
    GKC_ENGINE_INFO("{}", Debug::GetEngineName());
}

void Console::ShowFPS()
{
    double dt = Clock::GetDeltaTime();
    double fps = dt > 0.0 ? 1.0 / dt : 0.0;
    GKC_ENGINE_INFO("FPS: {:.1f}", fps);
}

//@todo Divide this into sections from an argument perspective, e.g. "scene", "entity", "script", "app", "sound", "debug"

void Console::DisplayHelp(const string& type)
{
    GKC_ENGINE_INFO("Available commands:");
    GKC_ENGINE_INFO(" save | load <file> | show_scene_list | pause");
    GKC_ENGINE_INFO(" create_static_object <name> | create_physics_object <name> | delete_entity <name>");
    GKC_ENGINE_INFO(" teleport_entity <name> <x> <y> / <target_name> | set_rotation_entity <name> <deg> | print_entity_list");
    GKC_ENGINE_INFO(" get_entity_info <name> | print_entity_list");
    GKC_ENGINE_INFO(" execute_script <name> | print_luastack | bind <key> <action>");
    GKC_ENGINE_INFO(" set_fullscreen | set_cheats <true|false> | clear | help");
    GKC_ENGINE_INFO(" play_sound <name> | stop_sound <name> | stop_all_sounds | print_sound_list");
    GKC_ENGINE_INFO(" show_fps | show_wireframe | show_colliders | show_debug_info");
}

void Console::ClearConsole()
{
    m_sink->Clear();
}

void Console::ExecuteCommand(const string& command) 
{
    if (command.empty())
        return;

    size_t indexOfSpace = command.find_first_of(" ");

    if (indexOfSpace == string::npos)
    {
        // Single-word command ("save", "help", ...) - nothing to split.
        auto wrapper = NoArgumentCommands(command);
        CommandSwitch(wrapper);
        return;
    }

    string literalCommand = command.substr(0, indexOfSpace);
    string arguments = command.substr(indexOfSpace + 1);

    if (arguments.empty())
    {
        auto wrapper = NoArgumentCommands(literalCommand);
        CommandSwitch(wrapper);
    }
    else
    {
        auto wrapper = ArgumentCommands(literalCommand, arguments);
        CommandSwitch(wrapper);
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

#define INVALID_CONVERION_ERROR(type) GKC_ENGINE_ERROR("failed to convert argument to '{}' type", #type)

void Console::SceneCommands(const CommandWrapper& wrapper) 
{
    const vector<string> args = GetListOfArguments(wrapper.args);

    switch(wrapper.type)
    {
        case CommandTypes::LoadScene: 
        {
            if (m_sceneManager == nullptr) { InvalidArgumentError("no SceneManager attached to the console!"); return; }
            if (args.empty()) { InvalidArgumentError("missing scene file path!"); return; }
            if(!m_sceneManager->LoadSpecificSceneFromFile(args[0])) {
                return;
            }
            break;
        }
        case CommandTypes::SaveCurrentScene: 
        {
            if (m_currentScene != nullptr)
                m_currentScene->Save();
            break;
        }
        case CommandTypes::ShowSceneList:
        {
            if (m_sceneManager == nullptr) { InvalidArgumentError("no SceneManager attached to the console!"); return; }
            m_sceneManager->ShowSceneList();
            break;
        }
        case CommandTypes::PauseToggleScene:
        {
            if (m_currentScene != nullptr)
                m_currentScene->Pause();
            break;
        }
    }
}

void Console::EntityCommands(const CommandWrapper& wrapper)
{
    if (m_currentScene == nullptr)
        return;

    const auto& manager = m_currentScene->GetECSManager();
    const vector<string> args = GetListOfArguments(wrapper.args);

    bool isCheatCommand = wrapper.type == CommandTypes::CreateStaticObject
        || wrapper.type == CommandTypes::CreatePhysicsObject
        || wrapper.type == CommandTypes::DeleteEntity
        || wrapper.type == CommandTypes::TeleportEntity
        || wrapper.type == CommandTypes::SetRotationEntity;


    if (isCheatCommand && !m_cheatsToggle) {
        GKC_ENGINE_ERROR("Cheats are disabled! Enable them with 'set_cheats true' first.");
        return;
    }

    switch(wrapper.type)
    {
        case CommandTypes::CreateStaticObject: 
        {
            if (args.empty()) { InvalidArgumentError("missing entity name!"); return; }
            m_currentScene->CreateStaticObject(args[0]);
            break;
        }
        case CommandTypes::CreatePhysicsObject:
        {
            if (args.empty()) { InvalidArgumentError("missing entity name!"); return; }
            m_currentScene->CreatePhysicsObject(args[0]);
            break;
        }
        case CommandTypes::DeleteEntity:
        {
            if (args.empty()) { InvalidArgumentError("missing entity name!"); return; }
            manager->DeleteEntityByName(args[0]);
            break;
        }
        case CommandTypes::TeleportEntity:
        {
            if (args.size() == 1) {
                InvalidArgumentError("missing coordinates! Usage: teleport_entity <source> <destinty> or teleport_entity <source> <x> <y>");
                return;
            }

            if(args.size() == 2) {
                if(args[0] == args[1]) {
                    GKC_ENGINE_ERROR("Cannot teleport an entity to itself!");
                    return;
                }
                if(!manager->EntityExists(args[0])) {
                    GKC_ENGINE_ERROR("Source '{}' doesn't exist!", args[0]);
                    return;
                }
                if(!manager->EntityExists(args[1])) {
                    GKC_ENGINE_ERROR("Destination '{}' doesn't exist!", args[1]);
                    return;
                }

                manager->TeleportEntityToEntityByName(args[0], args[1]);
                return;
            }
            

            SET_ARGUMENT_QUANTITY(3, args);
            try 
            {
                float x = std::stof(args[1]);
                float y = std::stof(args[2]);
                manager->TeleportEntityByName(args[0], x, y);
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

        case CommandTypes::CopyEntity:
        {
            SET_ARGUMENT_QUANTITY(2, args);
            string sourceName = args[0];
            string newName = args[1];
                
            EntityID sourceID = manager->GetEntityIDByName(sourceName);
            manager->CopyEntity(sourceID, newName);
        
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
            if (args.empty()) { InvalidArgumentError("missing entity name!"); return; }
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
    const vector<string> args = GetListOfArguments(wrapper.args);

    switch(wrapper.type)
    {
        case CommandTypes::ExecuteScript:
        {
            if (args.empty()) { InvalidArgumentError("missing script name!"); return; }
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
            SET_ARGUMENT_QUANTITY(2, args);
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
            if (m_currentScene != nullptr)
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
                return;
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
            if(args.empty()) {
                GKC_ENGINE_INFO("Type 'help <type>' for more information on a specific type.");
                GKC_ENGINE_INFO("Available types: scene, entity, script, app, sound, debug");
                return;
            }

            DisplayHelp(args);
            break;
        }
    }
}

void Console::SoundCommands(const CommandWrapper& wrapper)
{
    switch(wrapper.type)
    {
        case CommandTypes::PlaySound: {
            if (wrapper.args.empty()) { InvalidArgumentError("missing sound file name!"); return; }
            Managers::AudioManager::PlayAudioFile(wrapper.args);
            break;
        }
        case CommandTypes::StopSound: {
            if (wrapper.args.empty()) { InvalidArgumentError("missing sound file name!"); return; }
            Managers::AudioManager::StopSound(wrapper.args);
            break;
        }
        case CommandTypes::StopAllSounds: {
            Managers::AudioManager::StopAllSounds();
            break;
        }
        case CommandTypes::PrintSoundList: {
            Managers::AudioManager::PrintList();
            break;
        }
    }
}

void Console::DebugCommands(const CommandWrapper& wrapper) 
{
    if (m_currentScene == nullptr)
        return;

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
    std::istringstream stream(args);
    string token;
    while (stream >> token) {
        arguments.push_back(token);
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
    COM_WRAPPER_RETURN("clear", CommandTypes::ClearConsole);
    COM_WRAPPER_RETURN("print_sound_list", CommandTypes::PrintSoundList);
    COM_WRAPPER_RETURN("stop_all_sounds", CommandTypes::StopAllSounds);
    COM_WRAPPER_RETURN("set_fullscreen", CommandTypes::FullscreenToggle);
    COM_WRAPPER_RETURN("show_fps", CommandTypes::ShowFPSToggle);
    COM_WRAPPER_RETURN("show_wireframe", CommandTypes::ShowWireframeToggle);
    COM_WRAPPER_RETURN("show_colliders", CommandTypes::ShowCollidersToggle);
    COM_WRAPPER_RETURN("show_debug_info", CommandTypes::ShowDebugInformationToggle);

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
    COM_WRAPPER_RETURN("set_cheats", CommandTypes::CheatsToggle);
    COM_WRAPPER_RETURN("play_sound", CommandTypes::PlaySound);
    COM_WRAPPER_RETURN("stop_sound", CommandTypes::StopSound);
    COM_WRAPPER_RETURN("help", CommandTypes::HelpConsole);

    return CommandWrapper{};
}

SDL_Color Console::LevelToColor(spdlog::level::level_enum level)
{
    switch (level) {
        case spdlog::level::warn:     return YELLOW_COLOR;
        case spdlog::level::err:      return RED_COLOR;
        case spdlog::level::critical: return MAGENTA_COLOR;
        default:                      return WHITE_COLOR; // info / debug / trace
    }
}

#undef COM_CHECK
#undef COM_WRAPPER_RETURN
#undef SET_ARGUMENT_QUANTITY
#undef INVALID_CONVERION_ERROR