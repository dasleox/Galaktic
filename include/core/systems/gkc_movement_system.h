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

namespace Galaktic::ECS {
    class Entity;
    typedef unordered_map<EntityID, Entity> Entity_List;
}

namespace Galaktic::Core::Systems {
    class KeySystem;

    /**
     * @class MovementSystem
     * 
     * Manages player movement based on key inputs, updating entity positions accordingly.
     * A KeySystem reference is required to check for key states.
     */
    class MovementSystem final : public BaseSystem {
        public:
            /**
             * @param system Reference to KeySystem for input handling
             */
            explicit MovementSystem(KeySystem& system) : m_keySystem(system) {}

            /// @todo Implement diagonal movement normalization
            /// @todo Make a configurable system for key bindings
            /**
             * Updates the entities movement using delta time, with WASD keys for navigation.
             * @param list List of entities
             * @param dt Delta time
             */
            void Update(const ECS::Entity_List& list, float dt) override;
        private:
            KeySystem& m_keySystem;

            void ApplyJump(ECS::Entity& entity);
    };
}