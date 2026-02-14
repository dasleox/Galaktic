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
#include "core/gkc_logger.h"

namespace Galaktic::ECS {
    struct TransformComponent {
        TransformComponent() {}

        Render::Vec2 m_location{0.f, 0.f};
        Render::Vec2 m_size{50.f, 50.f};
        float m_rotation = 0.f;
    };

    struct HealthComponent {
        HealthComponent() {}
        HealthComponent(float maxHealth, bool canTakeDamage) 
            : m_canTakeDamage(canTakeDamage) {
            if(maxHealth <= 0.f && canTakeDamage) {
                GKC_ASSERT(maxHealth > 0.f, "Max health must be greater than 0");
                m_maxHealth = 100.f;
                m_currentHealth = 100.f;
            } else {
                if(!canTakeDamage) {
                    m_maxHealth = 100.f;
                    m_currentHealth = 100.f;
                } else {
                    m_maxHealth = maxHealth;
                    m_currentHealth = maxHealth;
                }
            }
        }
        
        float m_currentHealth = 100.f;
        float m_maxHealth = 100.f;
        bool m_canTakeDamage = true;
    };

    struct JumpComponent {
        JumpComponent() {}
        JumpComponent(float jumpHeight, bool canJump) : m_jumpHeight(jumpHeight), m_canJump(canJump) {
            // Ensure jump height is positive
            if(m_jumpHeight <= 0.f) m_jumpHeight = 100.f;
        }
        
        float m_jumpHeight = 100.f;
        bool m_canJump = true;
    };

    struct RigidBody {
        RigidBody() {}
        RigidBody(Render::Vec2 velocity, Render::Vec2 force, float mass)
            : m_velocity(velocity), m_force(force), m_mass(mass) {
            // Ensure mass is positive
            if(m_mass <= 0.f) {
                GKC_ASSERT(m_mass > 0.f, "Mass must be greater than 0");
                m_mass = 1.f;
            }
        }
        
        Render::Vec2 m_velocity{0.f, 0.f};
        Render::Vec2 m_force{0.f, 0.f};
        float m_mass = 1.f;
    };

    struct CollisionComponent {
        CollisionComponent() {}
        CollisionComponent(Render::Vec2 collisionBox, bool collidable)
            : m_collisionBox(collisionBox), m_collidable(collidable) {}
        
        Render::Vec2 m_collisionBox = {0.f, 0.f};
        bool m_collidable = true;
    };

    struct SpeedComponent {
        SpeedComponent() {}
        SpeedComponent(float maxSpeed) : m_maxSpeed(maxSpeed) {
            // Ensure max speed is non-negative
            if(m_maxSpeed < 0.f) m_maxSpeed = 1000.f;
        }
        
        float m_maxSpeed = 1000.f;
    };

    struct ColorComponent {
        ColorComponent() : m_color(GREY_COLOR) {}
        ColorComponent(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) : m_color{r, g, b, a} {}
        ColorComponent(SDL_Color color) : m_color(color) {}
        
        SDL_Color m_color;
    };

    struct NameComponent {
        NameComponent() {}
        NameComponent(const string& name) : m_name(name) {}
        string m_name = "";

        static void Write(ofstream& file, const NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file, Debug::WritingException);
            auto len = static_cast<Uint32>(comp.m_name.size());
            file.write(GKC_WRITE_BINARY(len), sizeof(len));
            file.write(comp.m_name.data(), len);
        }

        static void Read(ifstream& file, NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file, Debug::ReadingException);
            Uint32 len;
            file.read(GKC_READ_BINARY(len), sizeof(len));
            comp.m_name.resize(len);
            file.read(comp.m_name.data(), len);
        }

        static size_t Size(const NameComponent& comp) {
            return sizeof(Uint32) + comp.m_name.size();
        }
    };

    struct LightComponent {
        LightComponent() {}
        LightComponent(Render::Vec2 location, float watts, float radius, SDL_Color color)
            : m_location(location), m_watts(watts), m_radius(radius), m_color(color) {
            // Ensure watts and radius are positive
            if(m_watts <= 0.f) m_watts = 100.f;
            if(m_radius <= 0.f) m_radius = 1.f;
        }
        
        Render::Vec2 m_location = {0.f, 0.f};
        float m_watts = 100.f;
        float m_radius = 1.f;  // 1 meter = 32px
        SDL_Color m_color = WHITE_COLOR;
    };

    struct CameraComponent {
        CameraComponent() {}
        CameraComponent(Render::Vec2 location, EntityID entityToFollowID,
                        float zoom, float smoothing, bool isActive)
            : m_location(location), m_entityToFollowID(entityToFollowID), 
              m_zoom(zoom), m_smoothing(smoothing), m_isActive(isActive) {
            // Ensure zoom is positive
            if(m_zoom <= 0.f) m_zoom = 1.f;
            // Ensure smoothing is non-negative
            if(m_smoothing < 0.f) m_smoothing = 3.f;
        }
        
        Render::Vec2 m_location = {0.f, 0.f};
        EntityID m_entityToFollowID = InvalidEntity;
        float m_zoom = 1.f;
        float m_smoothing = 3.f;
        bool m_isActive = false;
    };

    struct TextureComponent {
        TextureComponent() {}
        TextureComponent(TextureID id) : m_id(id) {}
        TextureID m_id = 0;
    };

    struct ScriptComponent {
        ScriptComponent() {}
        ScriptComponent(ScriptID id) : m_id(id) {}
        ScriptID m_id = 0;
    };

    struct AnimationComponent {
        AnimationComponent() {}
        AnimationComponent(AnimationID id) : m_id(id) {}
        AnimationID m_id = 0;
    };

    struct VisibilityComponent {
        VisibilityComponent() {}
        VisibilityComponent(bool visible) : m_visible(visible) {}
        bool m_visible = true;
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
            : m_type(type)
            , m_size(size)
            , m_parentID(parentID)
            , m_isTag(isTag)
            , m_isPOD(isPOD)
            , m_sizeFunc(sizeFn)
            , m_serialize(serializeFn)
            , m_deserialize(deserializeFn)
        {}
        
        type_index m_type;
        size_t m_size;
        EntityID m_parentID;
        bool m_isTag;
        bool m_isPOD;

        // Modifiable Lambdas for writing/reading/size
        size_t (*m_sizeFunc)(const std::any&);
        void (*m_serialize)(const std::any&, ofstream&);
        void (*m_deserialize)(std::any&, ifstream&);
    };
}