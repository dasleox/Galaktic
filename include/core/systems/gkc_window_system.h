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
#include <core/systems/gkc_system.h>

namespace Galaktic::Core::Systems {
    /**
     * @class WindowSystem
     * @brief Manages window events like closing, minimizing, resize, etc.
     * 
     * WindowSystem is responsible for handling all window-related events
     * and executing appropriate actions based on those events.
     * For example when a window resize event is detected, it can adjust
     * the rendering viewport accordingly.
     * 
     * When a window close event is detected, it request the window to close
     * in the scene loop, this request is handled by a WindowManager.
     * 
     * @see gkc_window_man.h for more details on window management.
     */
    class WindowSystem : public BaseSystem {
        public:
            /**
            * @brief Execute actions depending on the window event received
            * @param e GKC_Event
            */
            void OnEvent(Events::GKC_Event &e) override;
    };
}