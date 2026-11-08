/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#include <pch.hpp>
#include <core/systems/gkc_key.h>
#include <core/systems/gkc_mouse_system.h>

namespace Galaktic::Config {
    /**
     * Holds the type of bind to store, key or mouse input
     */
    struct BindType {
        Core::Systems::Key keyBind = Core::Systems::Key::Unknown;
        Core::Systems::MouseClick mouseClick = Core::Systems::MouseClick::Unknown;
    };
    
    /**
     * @enum ActionType
     * @brief Identifies engine actions that can be assigned to input bindings.
     */
    enum class ActionType : uint32_t
    {
        Unknown = 0,
        MoveForward,
        MoveBackward,
        MoveLeft,
        MoveRight,
        
        Jump,
        Crouch,
        Duck,
        
        // MouseClicks Normally
        Attack,
        SecondaryAttack,
        SpecialAttack,
        
        // User Defined Actions (0-9)

        Action0,
        Action1,
        Action2,
        Action3,
        Action4,
        Action5,
        Action6,
        Action7,
        Action8,
        Action9,        
    };

    /**
     * Binds
     * 
     */
   class Bind {  
        public:
            BindType m_bind;
            bool m_isEnabled = true;
            ActionType m_actionType = ActionType::Unknown;
            string m_name;
            function<void()> m_functionCallback;    //< Assigned Later

        public:
            /**
             * @brief Default-constructs an inert/invalid bind.
             * @note Used as a safe fallback by BindConfigurator::GetBind when a
             *       bind name isn't found, instead of the old (undefined
             *       behavior) fallback of indexing the map with an integer 0.
             */
            Bind() = default;

            /**
             * @brief Creates an input binding for the supplied engine action.
             * @param bindName Name identifying the input binding
             * @param type Engine type identifier used by this operation
             * @param key Key
             */
            Bind(const string& bindName, ActionType type, const Core::Systems::Key& key) {
                m_name = bindName;
                m_actionType = type;
                m_bind.keyBind = key;
            }

            /**
             * @brief Creates an input binding for the supplied engine action.
             * @param bindName Name identifying the input binding
             * @param type Engine type identifier used by this operation
             * @param click Click
             */
            Bind(const string& bindName, ActionType type, const Core::Systems::MouseClick& click) {
                m_name = bindName;
                m_actionType = type;
                m_bind.mouseClick = click;
            }
    };

    typedef unordered_map<string, Bind> BindMap;

    /**
     * @class BindConfigurator
     * @brief Loads named input bindings and resolves their actions and callbacks.
     */
    class BindConfigurator {
        public:
            /**
             * @brief Loads input bindings from the supplied configuration filepath.
             * @param filepath Filepath to load or store
             */
            BindConfigurator(const path& filepath);
            /**
             * @brief Removes a named input binding.
             * @param bindName Name identifying the input binding
             */
            static void DeleteBind(const string &bindName);
            /**
             * @brief Finds an input binding assigned to the specified engine action.
             * @param type Engine type identifier used by this operation
             */
            static BindType GetBindByAction(ActionType type);
            /**
             * @brief Assigns the named input key or button to the requested engine action.
             * @param bindType Configured input key or mouse-button name
             * @param actionName Configured engine action name
             */
            static void BindAction(const string& bindType, const string& actionName);
            /**
             * @brief Returns the named input binding.
             * @param bindName Name identifying the input binding
             */
            static Bind GetBind(const string &bindName);
        private:
            static BindMap m_bindMap;
        private:
            /**
             * @brief Converts a configured input name into its keyboard or mouse binding.
             * @param str String to convert or inspect
             */
            static BindType ConvertStringToBindType(const string& str);
            /**
             * @brief Converts a configured action name to its engine action identifier.
             * @param str String to convert or inspect
             */
            static ActionType ConvertStringToActionType(const string& str);
            /**
             * @brief Checks whether the specified input binding is registered.
             * @param keyBind Key bind
             * @return true on success, false otherwise
             */
            static bool CheckBindExists(const Bind& keyBind);
            /**
             * @brief Checks whether the specified input binding is registered.
             * @param bindName Name identifying the input binding
             * @return true on success, false otherwise
             */
            static bool CheckBindExists(const string& bindName);
    };
}