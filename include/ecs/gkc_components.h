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
#include <render/gkc_shadow.h>
#include "core/gkc_logger.h"
#include "core/gkc_error.h"

namespace Galaktic::ECS {
    /**
     * @struct TransformComponent
     * @brief Stores an entity's world location, size, rotation in degrees and drawing order.
     */
    struct TransformComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        TransformComponent() {}

        Render::Vec2 location{0.f, 0.f};
        Render::Vec2 size{50.f, 50.f};
        float rotation = 0.f;
        int zIndex = 0;
    };

    /**
     * @struct HealthComponent
     * @brief Stores current and maximum health and whether damage is allowed.
     */
    struct HealthComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        HealthComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param maxHealthValue Max health value
         * @param canTakeDamage Can take damage
         */
        HealthComponent(float maxHealthValue, bool canTakeDamage) 
            : canTakeDamage(canTakeDamage) {
            if(maxHealthValue <= 0.f && canTakeDamage) {
                GKC_ASSERT(maxHealthValue > 0.f, "Max health must be greater than 0");
                maxHealth = 100.f;
                currentHealth = 100.f;
            } else {
                if(!canTakeDamage) {
                    maxHealth = 100.f;
                    currentHealth = 100.f;
                } else {
                    maxHealth = maxHealthValue;
                    currentHealth = maxHealth;
                }
            }
        }
        
        float currentHealth = 100.f;
        float maxHealth = 100.f;
        bool canTakeDamage = true;
    };

    /**
     * @struct JumpComponent
     * @brief Stores the jump force amount and whether jumping is allowed.
     */
    struct JumpComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        JumpComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param jumpHeight Jump height
         * @param canJump Can jump
         */
        JumpComponent(float jumpHeight, bool canJump) : jumpHeight(jumpHeight), canJump(canJump) {
            // Ensure jump height is positive
            if(jumpHeight <= 0.f) this->jumpHeight = 100.f;
        }
        
        float jumpHeight = 100.f;
        bool canJump = true;
    };

    /**
     * @struct RigidBody
     * @brief Stores velocity, accumulated force and mass for physics integration.
     */
    struct RigidBody {
        /**
         * @brief Initializes the component with its default values.
         */
        RigidBody() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param velocity Velocity
         * @param force Force
         * @param mass Mass
         */
        RigidBody(Render::Vec2 velocity, Render::Vec2 force, float mass)
            : velocity(velocity), force(force), mass(mass) {
            // Ensure mass is positive
            if(mass <= 0.f) {
                GKC_ASSERT(mass > 0.f, "Mass must be greater than 0");
                this->mass = 1.f;
            }
        }
        
        Render::Vec2 velocity{0.f, 0.f};
        Render::Vec2 force{0.f, 0.f};
        float mass = 1.f;
    };

    /**
     * @struct CollisionComponent
     * @brief Stores the collision box size, offset and collision-enabled flag.
     */
    struct CollisionComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        CollisionComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param collisionBox Collision box
         * @param collisionOffset Collision offset
         * @param collidable Collidable
         */
        CollisionComponent(Render::Vec2 collisionBox, Render::Vec2 collisionOffset = {0.f, 0.f}, bool collidable = true)
            : collisionBox(collisionBox), collisionOffset(collisionOffset), collidable(collidable) {}
        
        Render::Vec2 collisionBox = {0.f, 0.f};
        Render::Vec2 collisionOffset = {0.f, 0.f};
        bool collidable = true;
    };

    /**
     * @struct SpeedComponent
     * @brief Stores an entity's maximum movement speed.
     */
    struct SpeedComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        SpeedComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param maxSpeed Max speed
         */
        SpeedComponent(float maxSpeed) : maxSpeed(maxSpeed) {
            // Ensure max speed is non-negative
            if(maxSpeed < 0.f) this->maxSpeed = 100.f;
        }
        
        float maxSpeed = 100.f;
    };

    /**
     * @struct ColorComponent
     * @brief Stores the RGBA color used when an entity has no texture or animation.
     */
    struct ColorComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        ColorComponent() : color(GREY_COLOR) {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param r R
         * @param g G
         * @param b B
         * @param a A
         */
        ColorComponent(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) : color{r, g, b, a} {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param color RGBA color
         */
        ColorComponent(SDL_Color color) : color(color) {}
        
        SDL_Color color;
    };

    /**
     * @struct NameComponent
     * @brief Stores an entity's name and its binary serialization helpers.
     */
    struct NameComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        NameComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param name Name used to identify the requested entry
         */
        NameComponent(const std::string& name) : name(name) {}
        std::string name = "";

        /**
         * @brief Writes the component's binary representation to an open output stream.
         * @param file Open binary stream used for this operation
         * @param comp Component value to serialize or populate
         */
        static void Write(std::ofstream& file, const NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file);
            auto len = static_cast<Uint32>(comp.name.size());
            file.write(GKC_WRITE_BINARY(len), sizeof(len));
            file.write(comp.name.data(), len);
        }

        /**
         * @brief Reads the component's binary representation from an open input stream.
         * @param file Open binary stream used for this operation
         * @param comp Component value to serialize or populate
         */
        static void Read(std::ifstream& file, NameComponent& comp) {
            GKC_ENSURE_FILE_OPEN(file);
            Uint32 len;
            file.read(GKC_READ_BINARY(len), sizeof(len));
            comp.name.resize(len);
            file.read(comp.name.data(), len);
        }

        /**
         * @brief Returns the component's serialized size in bytes.
         * @param comp Component value to serialize or populate
         */
        static size_t Size(const NameComponent& comp) {
            return sizeof(Uint32) + comp.name.size();
        }
    };

    /**
     * @struct LightComponent
     * @brief Stores a point light's world location, intensity, radius in meters and color.
     */
    struct LightComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        LightComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param location Location
         * @param watts Watts
         * @param radius Light radius used by this operation
         * @param color RGBA color
         */
        LightComponent(Render::Vec2 location, float watts, float radius, SDL_Color color)
            : location(location), watts(watts), radius(radius), color(color) {
            // Ensure watts and radius are positive
            if(watts <= 0.f) this->watts = 100.f;
            if(radius <= 0.f) this->radius = 1.f;
        }
        
        Render::Vec2 location = {0.f, 0.f};
        float watts = 100.f;
        float radius = 1.f;  // 1 meter = 32px
        SDL_Color color = WHITE_COLOR;
    };

    /**
     * @struct CameraComponent
     * @brief Stores camera position, follow target, zoom, smoothing and active state.
     */
    struct CameraComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        CameraComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param location Location
         * @param entityToFollowID Entity to follow id
         * @param zoom Zoom
         * @param smoothing Smoothing
         * @param isActive Is active
         */
        CameraComponent(Render::Vec2 location, EntityID entityToFollowID,
                        float zoom, float smoothing, bool isActive)
            : location(location), entityToFollowID(entityToFollowID), 
              zoom(zoom), smoothing(smoothing), isActive(isActive) {
            // Ensure zoom is positive
            if(zoom <= 0.f) this->zoom = 1.f;
            // Ensure smoothing is non-negative
            if(smoothing < 0.f) this->smoothing = 3.f;
        }
        
        Render::Vec2 location = {0.f, 0.f};
        EntityID entityToFollowID = InvalidEntity;
        float zoom = 1.f;
        float smoothing = 3.f;
        bool isActive = false;
    };

    /**
     * @struct TextureComponent
     * @brief Associates an entity with a registered texture asset ID.
     */
    struct TextureComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        TextureComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param id Entity or asset ID used by this operation
         */
        TextureComponent(TextureID id) : id(id) {}
        TextureID id = 0;
    };

    /**
     * @struct ScriptComponent
     * @brief Associates an entity with a registered script asset ID.
     */
    struct ScriptComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        ScriptComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param id Entity or asset ID used by this operation
         */
        ScriptComponent(ScriptID id) : id(id) {}
        ScriptID id = 0;
    };

    /**
     * @struct AnimationComponent
     * @brief Associates an entity with a registered animation asset ID.
     */
    struct AnimationComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        AnimationComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param id Entity or asset ID used by this operation
         */
        AnimationComponent(AnimationID id) : id(id) {}
        AnimationID id = 0;
    };

    /**
     * @struct VisibilityComponent
     * @brief Controls whether the entity is rendered and contributes shadows or light.
     */
    struct VisibilityComponent {
        /**
         * @brief Initializes the component with its default values.
         */
        VisibilityComponent() {}
        /**
         * @brief Initializes the component from the supplied values.
         * @param visible Whether this entity should be rendered
         */
        VisibilityComponent(bool visible) : visible(visible) {}
        bool visible = true;
    };

    /**
     * @struct PhysicsObjectTag
     * @brief Marks an entity as a physics object.
     */
    struct PhysicsObjectTag {};
    /**
     * @struct StaticObjectTag
     * @brief Marks an entity as a static object.
     */
    struct StaticObjectTag {};
    /**
     * @struct LightTag
     * @brief Marks a dedicated light entity, which is excluded from sprite drawing.
     */
    struct LightTag {};
    /**
     * @struct PlayerTag
     * @brief Marks a player entity.
     */
    struct PlayerTag {};
    /**
     * @struct EnemyTag
     * @brief Marks an enemy entity.
     */
    struct EnemyTag {};
    /**
     * @struct CameraTag
     * @brief Marks a dedicated camera entity.
     */
    struct CameraTag {};

    /**
     * @struct ComponentTypeInfo
     * @brief Stores component metadata and callbacks used for binary serialization.
     */
    struct ComponentTypeInfo {
        /**
         * @brief Initializes the component from the supplied values.
         * @param type Engine type identifier used by this operation
         * @param size Fixed serialized size in bytes, or zero when sizeFn calculates it
         * @param parentID Entity ID used when registering this component type
         * @param isTag Whether this is an empty marker component
         * @param isPOD Whether the component supports direct binary copying
         * @param sizeFn Callback calculating the serialized byte count
         * @param serializeFn Callback writing the component to a binary stream
         * @param deserializeFn Callback reading the component from a binary stream
         */
        ComponentTypeInfo(std::type_index type,
            size_t size,
            EntityID parentID,
            bool isTag,
            bool isPOD,
            size_t (*sizeFn)(const std::any&),
            void (*serializeFn)(const std::any&, std::ofstream&),
            void (*deserializeFn)(std::any&, std::ifstream&)
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
        
        std::type_index type;
        size_t size;
        EntityID parentID;
        bool isTag;
        bool isPOD;

        // Modifiable Lambdas for writing/reading/size
        size_t (*sizeFunc)(const std::any&);
        void (*serialize)(const std::any&, std::ofstream&);
        void (*deserialize)(std::any&, std::ifstream&);
    };
}
