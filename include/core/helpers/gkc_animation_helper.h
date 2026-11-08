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

namespace Galaktic::Core::Managers {
    class ECS_Manager;
}
namespace Galaktic::Core::Helpers {
    /**
     * @class AnimationHelper
     * @brief Assigns registered animation assets through a borrowed ECS manager.
     */
    class AnimationHelper {
        public:
            /**
             * @brief Stores the borrowed ECS manager used by this helper.
             * @param ecsManager Borrowed ECS manager that must outlive this helper
             */
            AnimationHelper(Managers::ECS_Manager& ecsManager);
            /**
             * @brief Assigns a registered animation asset to an existing entity.
             * @param id Entity or asset ID used by this operation
             * @param animationName Registered animation filename, including its extension
             */
            void SetAnimationToEntity(EntityID id, const string& animationName);
        private:
            Managers::ECS_Manager& m_ecsManager;
    };
}