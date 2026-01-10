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
#include <core/systems/gkc_system.h>
typedef Uint32 EntityID;

namespace Galaktic::ECS {
    class Entity;
    typedef unordered_map<EntityID, Entity> Entity_List;
}

namespace Galaktic::Core::Systems {
    class KeySystem;

    /**
     * @class MovementSystem
     * @brief Manages all the movements of entities in the scene
     */
    class MovementSystem final : public BaseSystem {
        public:
            explicit MovementSystem(KeySystem& system) : m_keySystem(system) {}

            /**
             * @brief Updates the entities movement using delta time
             * @param list List of entities
             * @param dt Delta time
             */
            void Update(const ECS::Entity_List& list, float dt) override;
        private:
            KeySystem& m_keySystem;
    };
}