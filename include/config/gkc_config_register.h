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

#pragma once
#include <pch.hpp>

namespace Galaktic::Config
{
    /**
     * @class CallbackRegister
     */
    class CallbackRegister
    {
        private:
            unordered_map<string, function<void()>> m_callbackList;
        public:
            using Callback = function<void()>;
            
            static CallbackRegister& Get() 
            {
                static CallbackRegister instance;
                return instance;
            }

            /**
             * @brief Registers a callback under a name so it can later be
             *        found with Find(). Overwrites an existing callback of
             *        the same name (with a warning).
             * @param callbackName name to register the callback under
             * @param callback the callback to store
             */
            void RegisterCallback(const string& callbackName, Callback callback);

            /**
             * @brief Looks up a previously registered callback by name.
             * @param callbackName name the callback was registered under
             * @return pointer to the stored callback, or nullptr if none is
             *         registered under that name
             */
            const Callback* Find(const string& callbackName) const;
    };
}

#define GKC_REGISTER_CALLBACK(name, fn) \
    ::Galaktic::Config::CallbackRegister::Get().RegisterCallback(#name, fn)
