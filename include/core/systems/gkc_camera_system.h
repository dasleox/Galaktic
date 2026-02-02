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

namespace Galaktic::Core::Systems {
    // I got tired taking care of my siblings && doing this shit :,v

    /**
     * @class CameraSystem 
     * 
     * System for managing camera for the player, a camera entity is needed
     * for creating an instance, the camera \b NEEDS to have a CameraComponent,
     * inside the mentioned component a field called entityToFollowID_ has be to be 
     * filled with the entity to follow, normally the player's entitys's ID 
     * (normally ID is: 2, if a defaultscene was created). \n 
     * If modified change it accordingly with \c SetFollowEntity
     * 
     * Many cameras can exist on the scene and can become active if they have
     * the isActive_ field set to true in their CameraComponent, only one
     * camera can be active on the scene,
     */
    class CameraSystem final : public BaseSystem {
        public:
            explicit CameraSystem(ECS::Entity& camera);

            /**
             * Updates the camera position to the entity to follow, a list
             * is required to search which entity needs to have it's X and Y
             * coordinates, therefore a TransformComponent to follow correctly
             * 
             * Width and height of the screen are required to get the desired
             * location of the camera, smoothing to the camera can also be set
             * inside the CameraComponent of the active camera entity 
             * @param list Entity list
             * @param dt Delta time
             * @param width Width of the screen
             * @param height Height of the screen
             */
            void Update(const unordered_map<EntityID, ECS::Entity> &list, float dt, Uint32 width,
                Uint32 height) override;

            ECS::Entity& GetActiveCamera() { return m_activeCamera; }

            /**
             * Set the Follow Entity object
             * @param id 
             */
            void SetFollowEntity(EntityID id);
        private:
            /**
             * Helper funciton to find the primary camera (a.k.a active camera) in the entity list,
             * if found it sets the m_activeCamera to the found entity
             * @param list Entity list
             */
            void FindPrimaryCamera(const unordered_map<EntityID, ECS::Entity>& list);
            ECS::Entity& m_activeCamera;
    };
}
