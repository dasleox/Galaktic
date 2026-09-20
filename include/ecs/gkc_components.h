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
#include "core/gkc_logger.h"
#include "core/gkc_error.h"

namespace Galaktic::ECS {
    struct TransformComponent {
        TransformComponent() {}

        Render::Vec2 location{0.f, 0.f};
        Render::Vec2 size{50.f, 50.f};
        float rotation = 0.f;
    };

    struct HealthComponent {
        HealthComponent() {}
        HealthComponent(float maxHealth, bool canTakeDamage) 
            : canTakeDamage(canTakeDamage) {
            if(maxHealth <= 0.f && canTakeDamage) {
                GKC_ASSERT(maxHealth > 0.f, "Max health must be greater than 0");
                maxHealth = 100.f;
                currentHealth = 100.f;
            } else {
                if(!canTakeDamage) {
                    maxHealth = 100.f;
                    currentHealth = 100.f;
                } else {
                    maxHealth = maxHealth;
                    currentHealth = maxHealth;
                }
            }
        }
        
        float currentHealth = 100.f;
        float maxHealth = 100.f;
        bool canTakeDamage = true;
    };

    struct JumpComponent {
        JumpComponent() {}
        JumpComponent(float jumpHeight, bool canJump) : jumpHeight(jumpHeight), canJump(canJump) {
            // Ensure jump height is positive
            if(jumpHeight <= 0.f) jumpHeight = 100.f;
        }
        
        float jumpHeight = 100.f;
        bool canJump = true;
    };

    struct RigidBody {
        RigidBody() {}
        RigidBody(Render::Vec2 velocity, Render::Vec2 force, float mass)
            : velocity(velocity), force(force), mass(mass) {
            // Ensure mass is positive
            if(mass <= 0.f) {
                GKC_ASSERT(mass > 0.f, "Mass must be greater than 0");
                mass = 1.f;
            }
        }
        
        Render::Vec2 velocity{0.f, 0.f};
        Render::Vec2 force{0.f, 0.f};
        float mass = 1.f;
    };

    struct CollisionComponent {
        CollisionComponent() {}
        CollisionComponent(Render::Vec2 collisionBox, bool collidable)
            : collisionBox(collisionBox), collidable(collidable) {}
        
        Render::Vec2 collisionBox = {0.f, 0.f};
        bool collidable = true;
    };

    struct SpeedComponent {
        SpeedComponent() {}
        SpeedComponent(float maxSpeed) : maxSpeed(maxSpeed) {
            // Ensure max speed is non-negative
            if(maxSpeed < 0.f) maxSpeed = 1000.f;
        }
        
        float maxSpeed = 1000.f;
    };

    struct ColorComponent {
        ColorComponent() : color(GREY_COLOR) {}
        ColorComponent(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) : color{r, g, b, a} {}
        ColorComponent(SDL_Color color) : color(color) {}
        
        SDL_Color color;
    };

    struct NameComponent {
        NameComponent() {}
        NameComponent(const string& name) : name(name) {}
        string name = "";

        static void Write(ofstream& file, const NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file, WritingError);
            auto len = static_cast<Uint32>(comp.name.size());
            file.write(GKC_WRITE_BINARY(len), sizeof(len));
            file.write(comp.name.data(), len);
        }

        static void Read(ifstream& file, NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file, ReadingError);
            Uint32 len;
            file.read(GKC_READ_BINARY(len), sizeof(len));
            comp.name.resize(len);
            file.read(comp.name.data(), len);
        }

        static size_t Size(const NameComponent& comp) {
            return sizeof(Uint32) + comp.name.size();
        }
    };

    struct LightComponent {
        LightComponent() {}
        LightComponent(Render::Vec2 location, float watts, float radius, SDL_Color color)
            : location(location), watts(watts), radius(radius), color(color) {
            // Ensure watts and radius are positive
            if(watts <= 0.f) watts = 100.f;
            if(radius <= 0.f) radius = 1.f;
        }
        
        Render::Vec2 location = {0.f, 0.f};
        float watts = 100.f;
        float radius = 1.f;  // 1 meter = 32px
        SDL_Color color = WHITE_COLOR;
    };

    struct CameraComponent {
        CameraComponent() {}
        CameraComponent(Render::Vec2 location, EntityID entityToFollowID,
                        float zoom, float smoothing, bool isActive)
            : location(location), entityToFollowID(entityToFollowID), 
              zoom(zoom), smoothing(smoothing), isActive(isActive) {
            // Ensure zoom is positive
            if(zoom <= 0.f) zoom = 1.f;
            // Ensure smoothing is non-negative
            if(smoothing < 0.f) smoothing = 3.f;
        }
        
        Render::Vec2 location = {0.f, 0.f};
        EntityID entityToFollowID = InvalidEntity;
        float zoom = 1.f;
        float smoothing = 3.f;
        bool isActive = false;
    };

    struct TextureComponent {
        TextureComponent() {}
        TextureComponent(TextureID id) : id(id) {}
        TextureID id = 0;
    };

    struct ScriptComponent {
        ScriptComponent() {}
        ScriptComponent(ScriptID id) : id(id) {}
        ScriptID id = 0;
    };

    struct AnimationComponent {
        AnimationComponent() {}
        AnimationComponent(AnimationID id) : id(id) {}
        AnimationID id = 0;
    };

    struct VisibilityComponent {
        VisibilityComponent() {}
        VisibilityComponent(bool visible) : visible(visible) {}
        bool visible = true;
    };

    struct PhysicsObjectTag {};
    struct StaticObjectTag {};
    struct LightTag {};
    struct PlayerTag {};
    struct EnemyTag {};
    struct CameraTag {};

    struct ComponentTypeInfo {
        ComponentTypeInfo(type_index type,
            size_t size,
            EntityID parentID,
            bool isTag,
            bool isPOD,
            size_t (*sizeFn)(const any&),
            void (*serializeFn)(const any&, ofstream&),
            void (*deserializeFn)(any&, ifstream&)
        )
            : type(type)
            , size(size)
            , parentID(parentID)
            , isTag(isTag)
            , isPOD(isPOD)
            , sizeFunc(sizeFn)
            , serialize(serializeFn)
            , deserialize(deserializeFn)
        {}
        
        type_index type;
        size_t size;
        EntityID parentID;
        bool isTag;
        bool isPOD;

        // Modifiable Lambdas for writing/reading/size
        size_t (*sizeFunc)(const std::any&);
        void (*serialize)(const std::any&, ofstream&);
        void (*deserialize)(std::any&, ifstream&);
    };
}