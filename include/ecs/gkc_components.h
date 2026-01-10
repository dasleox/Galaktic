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

namespace Galaktic::ECS {
    struct TransformComponent {
        Render::Vec2 location_{0.f, 0.f};
        Render::Vec2 size_;
        float rotation_ = 0.f;
    };

    struct HealthComponent {
        float current_health = 1.f;
        float max_health_ = 1.f;
    };

    struct JumpComponent {
        float jump_height_ = 100.f;
    };

    struct RigidBody {
        Render::Vec2 velocity_ {0.f, 0.f};
        Render::Vec2 force_ {0.f, 0.f};
        float mass_ = 1.f;
        bool use_gravity_ = true;
    };

    struct CollisionComponent {
        Render::Vec2 collision_box_ = {0.f, 0.f};
        bool collidable_ = false;
    };

    struct SpeedComponent {
        float max_speed_ = 1000.f;
    };

    struct ColorComponent {
        SDL_Color color_ = GREY_COLOR;
    };

    struct NameComponent {
        std::string name_;
    };

    struct LightComponent {
        Render::Vec2 position_;
        float watts_ = 100.f;
        float radius = 1.f;  // 1 meter = 32px
        SDL_Color color_ = WHITE_COLOR;
    };

    struct PhysicsObjectTag {};
    struct StaticObjectTag {};
    struct LightTag {};
    struct PlayerTag {};
    struct EnemyTag {};
}
