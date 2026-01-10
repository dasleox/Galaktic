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

typedef Uint32 EntityID;
namespace Galaktic::Core::Events { class GKC_Event; }
namespace Galaktic::ECS {
    class Entity;
}

namespace Galaktic::Core::Systems {
    /**
     * @class BaseSystem
     * @brief Base class for systems
     */
    class BaseSystem {
        public:
            virtual ~BaseSystem() = default;
            virtual void Update(float dt) {}
            virtual void Update(const unordered_map<EntityID, ECS::Entity>& list, float dt) {}
            virtual void OnEvent(Events::GKC_Event& e) {}
    };

    typedef unordered_map<string, shared_ptr<BaseSystem>> System_List;
}

constexpr int GKC_SYSTEMS_COUNTER = 6;