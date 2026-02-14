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
    union BindType {
        Core::Systems::Key m_keyBind;
        Core::Systems::MouseClick m_mouseClick;
    };

    class Bind {  
        public:
            BindType m_bind;
            bool m_isEnabled = true;
            string m_name;
            string m_description;
            luabridge::LuaRef m_callback = nullptr;

            Bind(const string& bindName, const Core::Systems::Key& key) {
                m_name = bindName;
                m_bind.m_keyBind = key;
            }

            Bind(const string& bindName, const Core::Systems::MouseClick& click) {
                m_name = bindName;
                m_bind.m_mouseClick = click;
            }
    };

    typedef unordered_map<string, Bind> BindMap;

    class BindConfigurator {
        public:
            static void CreateKeyBind(const string& bindName, const Core::Systems::Key& key);
            static void CreateMouseBind(const string& bindName, const Core::Systems::MouseClick& click);

            static void DeleteBind(const string& bindName);

            static void BindAction(const string& functionName, const string& bindName);

            static Bind GetBind(const string& bindName);
        private:
            static BindMap m_bindMap;
        private:
            static bool CheckBindExists(const Bind& keyBind);
            static bool CheckBindExists(const string& bindName);
    };
}