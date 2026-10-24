#include <config/gkc_config.h>
#include <config/gkc_config_parser.h>
#include <core/gkc_logger.h>
#include <script/gkc_library.h>
#include <core/gkc_error.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_console.h>

using namespace Galaktic;
using namespace Galaktic::Config;
using namespace Galaktic::Core::Systems;

BindMap BindConfigurator::m_bindMap;

BindConfigurator::BindConfigurator(const path& filepath) {
    GKC_CHECK_PATH_INTEGRITY(filepath);

    if (!ConfigParser::ParseFile(filepath))
        return;

    for (const auto& entry : ConfigParser::GetBindEntryList()) {
        if (entry.bindKey.empty() || entry.actionName.empty())
            continue;

        BindAction(entry.bindKey, entry.actionName);
    }
}

BindType BindConfigurator::GetBindByAction(ActionType type) {
    for (const auto& pair : m_bindMap) {
        if (pair.second.m_actionType == type) {
            return pair.second.m_bind;
        }
    }
    return BindType{};
}

void BindConfigurator::DeleteBind(const string& bindName) {
    if(m_bindMap.contains(bindName)) {
        m_bindMap.erase(bindName);
        return;
    }
    Debug::Logger::LogErrorWithType(ErrorType::BindNotFound, "Bind '{}' to delete doesn't exist!", bindName);
}

#define STR_TO_ACTION(actionStr, actionEnum)     \
    do {                                         \
        if((str) == (actionStr)) {               \
            return ActionType::actionEnum;       \
        }                                        \
    } while(0)

ActionType BindConfigurator::ConvertStringToActionType(const string& str)
{
    STR_TO_ACTION("move_forward",    MoveForward);
    STR_TO_ACTION("move_backward",   MoveBackward);
    STR_TO_ACTION("move_left",       MoveLeft);
    STR_TO_ACTION("move_right",      MoveRight);

    STR_TO_ACTION("jump",            Jump);
    STR_TO_ACTION("crouch",          Crouch);
    STR_TO_ACTION("duck",            Duck);

    STR_TO_ACTION("attack",          Attack);
    STR_TO_ACTION("secondary_attack",SecondaryAttack);
    STR_TO_ACTION("special_attack",  SpecialAttack);

    STR_TO_ACTION("action0",         Action0);
    STR_TO_ACTION("action1",         Action1);
    STR_TO_ACTION("action2",         Action2);
    STR_TO_ACTION("action3",         Action3);
    STR_TO_ACTION("action4",         Action4);
    STR_TO_ACTION("action5",         Action5);
    STR_TO_ACTION("action6",         Action6);
    STR_TO_ACTION("action7",         Action7);
    STR_TO_ACTION("action8",         Action8);
    STR_TO_ACTION("action9",         Action9);

    return ActionType::Unknown;
}

BindType BindConfigurator::ConvertStringToBindType(const std::string& str)
{
    BindType bind;

    if      (str == "left_click")   { bind.mouseClick = MouseClick::LeftClick;   return bind; }
    else if (str == "right_click")  { bind.mouseClick = MouseClick::RightClick;  return bind; }
    else if (str == "middle_click") { bind.mouseClick = MouseClick::MiddleClick; return bind; }

    else if (str == "return")    { bind.keyBind = Key::Return;    return bind; }
    else if (str == "escape")    { bind.keyBind = Key::Escape;    return bind; }
    else if (str == "backspace") { bind.keyBind = Key::Backspace; return bind; }
    else if (str == "tab")       { bind.keyBind = Key::Tab;       return bind; }
    else if (str == "space")     { bind.keyBind = Key::Space;     return bind; }

    else if (str == "0") { bind.keyBind = Key::Key0; return bind; }
    else if (str == "1") { bind.keyBind = Key::Key1; return bind; }
    else if (str == "2") { bind.keyBind = Key::Key2; return bind; }
    else if (str == "3") { bind.keyBind = Key::Key3; return bind; }
    else if (str == "4") { bind.keyBind = Key::Key4; return bind; }
    else if (str == "5") { bind.keyBind = Key::Key5; return bind; }
    else if (str == "6") { bind.keyBind = Key::Key6; return bind; }
    else if (str == "7") { bind.keyBind = Key::Key7; return bind; }
    else if (str == "8") { bind.keyBind = Key::Key8; return bind; }
    else if (str == "9") { bind.keyBind = Key::Key9; return bind; }

    else if (str == "a") { bind.keyBind = Key::A; return bind; }
    else if (str == "b") { bind.keyBind = Key::B; return bind; }
    else if (str == "c") { bind.keyBind = Key::C; return bind; }
    else if (str == "d") { bind.keyBind = Key::D; return bind; }
    else if (str == "e") { bind.keyBind = Key::E; return bind; }
    else if (str == "f") { bind.keyBind = Key::F; return bind; }
    else if (str == "g") { bind.keyBind = Key::G; return bind; }
    else if (str == "h") { bind.keyBind = Key::H; return bind; }
    else if (str == "i") { bind.keyBind = Key::I; return bind; }
    else if (str == "j") { bind.keyBind = Key::J; return bind; }
    else if (str == "k") { bind.keyBind = Key::K; return bind; }
    else if (str == "l") { bind.keyBind = Key::L; return bind; }
    else if (str == "m") { bind.keyBind = Key::M; return bind; }
    else if (str == "n") { bind.keyBind = Key::N; return bind; }
    else if (str == "o") { bind.keyBind = Key::O; return bind; }
    else if (str == "p") { bind.keyBind = Key::P; return bind; }
    else if (str == "q") { bind.keyBind = Key::Q; return bind; }
    else if (str == "r") { bind.keyBind = Key::R; return bind; }
    else if (str == "s") { bind.keyBind = Key::S; return bind; }
    else if (str == "t") { bind.keyBind = Key::T; return bind; }
    else if (str == "u") { bind.keyBind = Key::U; return bind; }
    else if (str == "v") { bind.keyBind = Key::V; return bind; }
    else if (str == "w") { bind.keyBind = Key::W; return bind; }
    else if (str == "x") { bind.keyBind = Key::X; return bind; }
    else if (str == "y") { bind.keyBind = Key::Y; return bind; }
    else if (str == "z") { bind.keyBind = Key::Z; return bind; }

    else if (str == "f1")  { bind.keyBind = Key::F1;  return bind; }
    else if (str == "f2")  { bind.keyBind = Key::F2;  return bind; }
    else if (str == "f3")  { bind.keyBind = Key::F3;  return bind; }
    else if (str == "f4")  { bind.keyBind = Key::F4;  return bind; }
    else if (str == "f5")  { bind.keyBind = Key::F5;  return bind; }
    else if (str == "f6")  { bind.keyBind = Key::F6;  return bind; }
    else if (str == "f7")  { bind.keyBind = Key::F7;  return bind; }
    else if (str == "f8")  { bind.keyBind = Key::F8;  return bind; }
    else if (str == "f9")  { bind.keyBind = Key::F9;  return bind; }
    else if (str == "f10") { bind.keyBind = Key::F10; return bind; }
    else if (str == "f11") { bind.keyBind = Key::F11; return bind; }
    else if (str == "f12") { bind.keyBind = Key::F12; return bind; }

    else if (str == "left")  { bind.keyBind = Key::Left;  return bind; }
    else if (str == "right") { bind.keyBind = Key::Right; return bind; }
    else if (str == "up")    { bind.keyBind = Key::Up;    return bind; }
    else if (str == "down")  { bind.keyBind = Key::Down;  return bind; }

    else if (str == "left_shift")  { bind.keyBind = Key::LeftShift;  return bind; }
    else if (str == "right_shift") { bind.keyBind = Key::RightShift; return bind; }
    else if (str == "left_ctrl")   { bind.keyBind = Key::LeftCtrl;   return bind; }
    else if (str == "right_ctrl")  { bind.keyBind = Key::RightCtrl;  return bind; }
    else if (str == "left_alt")    { bind.keyBind = Key::LeftAlt;    return bind; }
    else if (str == "right_alt")   { bind.keyBind = Key::RightAlt;   return bind; }
    else if (str == "left_meta")   { bind.keyBind = Key::LeftMeta;   return bind; }
    else if (str == "right_meta")  { bind.keyBind = Key::RightMeta;  return bind; }

    else if (str == "insert")    { bind.keyBind = Key::Insert;   return bind; }
    else if (str == "delete")    { bind.keyBind = Key::Delete;   return bind; }
    else if (str == "home")      { bind.keyBind = Key::Home;     return bind; }
    else if (str == "end")       { bind.keyBind = Key::End;      return bind; }
    else if (str == "page_up")   { bind.keyBind = Key::PageUp;   return bind; }
    else if (str == "page_down") { bind.keyBind = Key::PageDown; return bind; }

    else if (str == "keypad_0") { bind.keyBind = Key::KeyPad0; return bind; }
    else if (str == "keypad_1") { bind.keyBind = Key::KeyPad1; return bind; }
    else if (str == "keypad_2") { bind.keyBind = Key::KeyPad2; return bind; }
    else if (str == "keypad_3") { bind.keyBind = Key::KeyPad3; return bind; }
    else if (str == "keypad_4") { bind.keyBind = Key::KeyPad4; return bind; }
    else if (str == "keypad_5") { bind.keyBind = Key::KeyPad5; return bind; }
    else if (str == "keypad_6") { bind.keyBind = Key::KeyPad6; return bind; }
    else if (str == "keypad_7") { bind.keyBind = Key::KeyPad7; return bind; }
    else if (str == "keypad_8") { bind.keyBind = Key::KeyPad8; return bind; }
    else if (str == "keypad_9") { bind.keyBind = Key::KeyPad9; return bind; }

    else if (str == "keypad_plus")     { bind.keyBind = Key::KeyPadPlus;     return bind; }
    else if (str == "keypad_minus")    { bind.keyBind = Key::KeyPadMinus;    return bind; }
    else if (str == "keypad_multiply") { bind.keyBind = Key::KeyPadMultiply; return bind; }
    else if (str == "keypad_divide")   { bind.keyBind = Key::KeyPadDivide;   return bind; }
    else if (str == "keypad_enter")    { bind.keyBind = Key::KeyPadEnter;    return bind; }
    else if (str == "keypad_decimal")  { bind.keyBind = Key::KeyPadDecimal;  return bind; }

    bind.keyBind = Key::Unknown;
    return bind;
}

void BindConfigurator::BindAction(const string& bindType, const string& actionName) 
{
    auto type = ConvertStringToActionType(actionName);
    auto eBindType = ConvertStringToBindType(bindType);

    if(eBindType.keyBind == Key::Unknown && eBindType.mouseClick == MouseClick::Unknown)
    {
        Debug::Logger::LogErrorWithType(ErrorType::BindNotFound, "'{}' is not a recognized key or mouse click!", bindType);
        return;
    }

    if(type == ActionType::Unknown)
    {
        Debug::Logger::LogErrorWithType(ErrorType::ActionNotFound, "'{}' is not a recognized action!", actionName);
        return;
    }

    if (CheckBindExists(actionName)) {
        GKC_ENGINE_INFO("Action '{}' was already bound, overwriting previous bind.", actionName);
    }

    switch(eBindType.mouseClick) 
    {
        case MouseClick::LeftClick:
        case MouseClick::RightClick:
        case MouseClick::MiddleClick:
        {
            Bind bind = Bind(actionName, type, eBindType.mouseClick);
            m_bindMap.insert_or_assign(actionName, bind);
            return;
        }
        default:
            break;
    }

    Bind bind = Bind(actionName, type, eBindType.keyBind);
    m_bindMap.insert_or_assign(actionName, bind);
}

Bind BindConfigurator::GetBind(const string& bindName) {
    if (CheckBindExists(bindName)) return m_bindMap.find(bindName)->second;
    Debug::Logger::LogErrorWithType(ErrorType::BindNotFound, "Bind '{}' doesn't exist!", bindName);
    return Bind(bindName, ActionType::Unknown, Key::Unknown);
}

bool BindConfigurator::CheckBindExists(const Bind& keyBind) {
    return m_bindMap.contains(keyBind.m_name);
}

bool BindConfigurator::CheckBindExists(const string& bindName) {
    return m_bindMap.contains(bindName);
}