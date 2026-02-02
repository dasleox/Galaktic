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
#include "core/gkc_exception.h"

namespace Galaktic::ECS {
    struct TransformComponent {
        Render::Vec2 location_{0.f, 0.f};
        Render::Vec2 size_{50.f, 50.f};
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
        string name_;

         static void Write(ofstream& file, const NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file, Debug::WritingException);
             auto len = static_cast<Uint32>(comp.name_.size());
             file.write(GKC_WRITE_BINARY(len), sizeof(len));
             file.write(comp.name_.data(), len);
        }

        static void Read(ifstream& file, NameComponent& comp) {
             GKC_ENSURE_FILE_OPEN(file, Debug::ReadingException);
             Uint32 len;
             file.read(GKC_READ_BINARY(len), sizeof(len));
             comp.name_.resize(len);
             file.read(comp.name_.data(), len);
         }

        static size_t Size(const NameComponent& comp) {
             return sizeof(Uint32) + comp.name_.size();
        }
    };

    struct LightComponent {
        Render::Vec2 location_ = {0.f, 0.f};
        float watts_ = 100.f;
        float radius = 1.f;  // 1 meter = 32px
        SDL_Color color_ = WHITE_COLOR;
    };

    struct CameraComponent {
        Render::Vec2 location_ = {0.f, 0.f};
        EntityID entityToFollowID_ = InvalidEntity;
        float zoom_ = 1.f;
        float smoothing_ = 3.f;
        bool isActive_ = false;
    };

    struct TextureComponent {
        TextureID id_;
    };

    struct PhysicsObjectTag {};
    struct StaticObjectTag {};
    struct LightTag {};
    struct PlayerTag {};
    struct EnemyTag {};
    struct CameraTag {};\

    struct ComponentTypeInfo {
        type_index type_;
        size_t size_;
        EntityID parentID_;
        bool isTag_;
        bool isPOD_;

        // Modifiable Lambdas for writing/reading/size
        size_t (*size_func) (const std::any&);
        void (*serialize)(const std::any&, ofstream&);
        void (*deserialize)(std::any&, ifstream&);

        ComponentTypeInfo(type_index type,
            size_t size,
            EntityID parentID,
            bool isTag,
            bool isPOD,
            size_t (*sizeFn)(const any&),
            void (*serializeFn)(const any&, ofstream&),
            void (*deserializeFn)(any&, ifstream&)
        )
            : type_(type)
            , size_(size)
            , parentID_(parentID)
            , isTag_(isTag)
            , isPOD_(isPOD)
            , size_func(sizeFn)
            , serialize(serializeFn)
            , deserialize(deserializeFn)
        {}
    };
}
