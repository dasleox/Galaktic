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
#include "gkc_key.h"

/// @todo Comment and document this file

namespace Galaktic::Core::Systems {
    /**
     * @class UISystem
     * @brief Manages all UI related events
     */
    class UISystem final : public BaseSystem {
        public:
            /**
             * @param system KeySystem reference
             */
            explicit UISystem(KeySystem& system) : m_system(system) {}

            /**
             * @brief Execute UI actions depending on the key pressed
             * @param e GKC_Event
             */
            void OnEvent(Events::GKC_Event &e) override;
        private:
            KeySystem& m_system;
    };
}
