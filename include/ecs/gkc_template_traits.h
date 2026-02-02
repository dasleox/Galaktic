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
#include <ecs/gkc_components.h>

namespace Galaktic::ECS {
    template<typename T>
    inline constexpr bool IsTag = false;
    template<> inline constexpr bool IsTag<ECS::PlayerTag> = true;
    template<> inline constexpr bool IsTag<ECS::StaticObjectTag> = true;
    template<> inline constexpr bool IsTag<ECS::PhysicsObjectTag> = true;
    template<> inline constexpr bool IsTag<ECS::CameraTag> = true;
    template<> inline constexpr bool IsTag<ECS::LightTag> = true;
    template<> inline constexpr bool IsTag<ECS::EnemyTag> = true;

    template<typename Y>
    inline constexpr bool IsNonPOD = false;
    template<> inline constexpr bool IsNonPOD<ECS::NameComponent> = true;

}