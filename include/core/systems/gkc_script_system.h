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
#include <core/systems/gkc_system.h>

namespace Galaktic::Core::Managers {
    class ECS_Manager;
}

namespace Galaktic::Core::Systems {
    class ScriptSystem final : public BaseSystem {
        public:
            ScriptSystem(Managers::ECS_Manager& manager);

            void OnEvent(Events::GKC_Event& event) override;
        private:
            Managers::ECS_Manager& m_ecsManager;
    };
}

#define GKC_CHECK_RUN_SCRIPT(script, id)                                            \
    if(script != nullptr) {                                                         \
        script->RunScript();                                                        \
    } else {                                                                        \
        GKC_ENGINE_ERROR("Script to run (ID: {}) doesn't exists", id); \
    }
