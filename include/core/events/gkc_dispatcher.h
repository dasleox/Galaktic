/*
  Galaktic Engine
  Copyright (C) 2025 SummerChip

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
#include <core/events/gkc_event.h>

namespace Galaktic::Core::Events {
    /**
     * @class GKC_EventDispatcher gkc_dispatcher.h
     * @brief Dispatch events
     * @todo Make events queued in this class
     */
    class GKC_EventDispatcher {
        public:
            explicit GKC_EventDispatcher(GKC_Event& event) : m_event(event) {};

            /**
             * @brief Dispatch events and check if they were handled
             * @tparam T event typename (GKC_Event children)
             * @param func function/lambda
             * @return true if the function passed is handled, false is the event wasn't handled
             */
            template<typename T>
            bool DispatchEvent(function<bool(T&)> func) {
                if (m_event.GetEventType() == T::GetStaticType()) {
                    m_event.m_handled = func(static_cast<T&>(m_event));
                    return true;
                }
                return false;
            }
        private:
            GKC_Event& m_event;
    };
}