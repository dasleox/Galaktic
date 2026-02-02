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

typedef Uint32 EntityID;
namespace Galaktic::Core::Events { class GKC_Event; }
namespace Galaktic::ECS {
    class Entity;
}

namespace Galaktic::Core::Systems {
    /**
     * @class BaseSystem
     * @brief Base class for systems
     * 
     * This class provides a common interface for all systems in the ECS architecture.
     * Systems can override the \c Update and \c OnEvent methods to implement their specific behavior.
     * Three functions exists for \c Update, one has an entity list and a delta time parameter,
     * another has only a delta time parameter and the last one has an entity list, delta time and
     * width and height parameters that are exclusively used for the \c CameraSystem (by now).
     * 
     * \c OnEvent function is used to handle events, if the system needs to respond to events once
     * at a time, a dispatcher has be to created on \c OnEvent function implementation in order
     * to responde to events inside the inherited system.
     */
    class BaseSystem {
        public:
            virtual ~BaseSystem() = default;

            /**
             * Updates the system with the given delta time.
             * Use this function to update the system without entity context.
             * @param dt Delta time
             */
            virtual void Update(float dt) {}
            virtual void Update(const unordered_map<EntityID, ECS::Entity>& list, float dt) {}

            /**
             * Updates the system with the given entity list and delta time.
             * @param list List of entities
             * @param dt Delta time
             */
            virtual void Update(const unordered_map<EntityID, ECS::Entity>& list, float dt,
                Uint32 width, Uint32 height) {}
            
            /**
             * Handles an event, when an event has happened, a dispatcher has to be created
             * in order to respond to events inside the inherited system overriding this function.
             * @param e GKC_Event
             */
            virtual void OnEvent(Events::GKC_Event& e) {}
    };

    typedef unordered_map<string, shared_ptr<BaseSystem>> System_List;
}

constexpr int GKC_SYSTEMS_COUNTER = 8;