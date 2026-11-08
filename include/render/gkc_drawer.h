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
#include <cmath>
#include <pch.hpp>
#include <render/gkc_shadow.h>


namespace Galaktic::ECS {
    class Entity;
}
namespace Galaktic::Core::Systems {
    class CameraSystem;
}

namespace Galaktic::Render {
    /**
     * @class Drawer
     * @brief Helper to render different things on the scene
     */
    class Drawer {
        public:
            /**
             * @brief Draws all entities
             * @param list List of entities to draw
             * @param renderer SDL_Renderer
             * @param cameraSystem CameraSystem reference
             */
            static void DrawEntities(const unordered_map<EntityID, ECS::Entity>& list, SDL_Renderer* renderer,
                Core::Systems::CameraSystem& cameraSystem);

            /**
             * @brief Draws collision-box debug overlays in camera coordinates.
             * @param list Entity handles to inspect or draw
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param cameraSystem Camera system providing the active camera
             */
            static void DrawColliders(const unordered_map<EntityID, ECS::Entity>& list, SDL_Renderer* renderer,
                Core::Systems::CameraSystem& cameraSystem);

            /**
             * @brief Draws rotated entity outlines in camera coordinates.
             * @param list Entity handles to inspect or draw
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param cameraSystem Camera system providing the active camera
             */
            static void DrawWireframes(const unordered_map<EntityID, ECS::Entity>& list, SDL_Renderer* renderer,
                Core::Systems::CameraSystem& cameraSystem);

            static constexpr int MaxShadowRadiusPixels = 1024;

            /** Apply after world drawing, before UI/debug overlays. Dimensions
             *  are logical pixels. Returns false with SDL_GetError() on failure;
             *  an incomplete light map is never composited. Ambient is [0, 1].
             */
            static bool DrawLighting(const unordered_map<EntityID, ECS::Entity>& list,
                SDL_Renderer* renderer, Core::Systems::CameraSystem& cameraSystem,
                LightingState& state, int viewWidth, int viewHeight, float ambientLight);

        private:
            using AlphaMask = Detail::ShadowAlphaMask;
            using ShadowOccluder = Detail::ShadowOccluder;
            using SceneLight = Detail::SceneLight;

            /**
             * @brief Creates or resizes reusable lighting buffers and the output texture.
             * @param state Reusable lighting buffers owned by the scene
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param width Width in pixels
             * @param height Height in pixels
             * @return true on success, false otherwise
             */
            static bool PrepareLighting(LightingState& state, SDL_Renderer* renderer, int width, int height);
            /**
             * @brief Collects finite, visible lights whose radii overlap the logical camera view.
             * @param list Entity handles to inspect or draw
             * @param cameraX Camera's horizontal world position
             * @param cameraY Camera's vertical world position
             * @param state Reusable lighting buffers owned by the scene
             * @return true on success, false otherwise
             */
            static bool CollectLights(const unordered_map<EntityID, ECS::Entity>& list,
                float cameraX, float cameraY, LightingState& state);
            /**
             * @brief Refreshes and returns a borrowed alpha mask once per source surface per frame.
             * @param surface Borrowed source surface
             * @param state Reusable lighting buffers owned by the scene
             */
            static const AlphaMask* GetAlphaMask(SDL_Surface* surface, LightingState& state);
            /**
             * @brief Collects visible shadow casters overlapping the active lights and resolves their alpha masks.
             * @param list Entity handles to inspect or draw
             * @param cameraX Camera's horizontal world position
             * @param cameraY Camera's vertical world position
             * @param state Reusable lighting buffers owned by the scene
             * @return true on success, false otherwise
             */
            static bool CollectOccluders(const unordered_map<EntityID, ECS::Entity>& list,
                float cameraX, float cameraY, LightingState& state);
            /**
             * @brief Rasterizes rotated caster silhouettes into the current light's pixel grid.
             * @param light Point light prepared for the current frame
             * @param state Reusable lighting buffers owned by the scene
             */
            static void BuildOcclusion(const SceneLight& light, LightingState& state);
            /**
             * @brief Marks visible cells in one sector, recursively splitting visibility at opaque cells.
             * @param state Reusable lighting buffers owned by the scene
             * @param radius Light radius used by this operation
             * @param row First row to visit in the light's sector
             * @param startSlope First visible slope in the sector
             * @param endSlope Last visible slope in the sector
             * @param xx Sector transform coefficient mapping local X to grid X
             * @param xy Sector transform coefficient mapping local Y to grid X
             * @param yx Sector transform coefficient mapping local X to grid Y
             * @param yy Sector transform coefficient mapping local Y to grid Y
             */
            static void CastLight(LightingState& state, int radius, int row,
                float startSlope, float endSlope, int xx, int xy, int yx, int yy);
            /**
             * @brief Calculates squared-distance falloff, returning zero outside the light radius.
             * @param light Point light prepared for the current frame
             * @param distanceSquared Squared distance from the light in logical pixels
             */
            static float CalculateLightIntensity(const SceneLight& light, float distanceSquared);
            /**
             * @brief Adds a light's visible RGB contribution to the camera's brightness buffer.
             * @param light Point light prepared for the current frame
             * @param state Reusable lighting buffers owned by the scene
             */
            static void AccumulateLight(const SceneLight& light, LightingState& state);

            /**
             * @brief Draws a rotated colored rectangle and restores the previous draw blend mode.
             * @param rect Destination rectangle in renderer coordinates
             * @param entity Entity
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             */
            static void RenderColor(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer);
            /**
             * @brief Draws the entity's texture, using the missing-texture fallback when unavailable.
             * @param rect Destination rectangle in renderer coordinates
             * @param entity Entity
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             */
            static void RenderTexture(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer);
            /**
             * @brief Draws the current animation frame or falls back to color when the asset is missing.
             * @param rect Destination rectangle in renderer coordinates
             * @param entity Entity
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             */
            static void RenderAnimation(SDL_FRect rect, ECS::Entity& entity, SDL_Renderer* renderer);
            /**
             * @brief Writes the four rotated rectangle vertices, including normalized vertex colors.
             * @param rect Destination rectangle in renderer coordinates
             * @param rotation Rotation in degrees around the rectangle center
             * @param color RGBA color
             * @param outVerts Output array containing the four rectangle vertices
             */
            static void CalculateVertices(SDL_FRect rect, float rotation, SDL_Color color, SDL_Vertex outVerts[4]);
            /**
             * @brief Returns entity handles sorted by transform z-index, then ID; handles without transforms come last.
             * @param list Entity handles to inspect or draw
             */
            static const std::vector<std::pair<EntityID, ECS::Entity>> OrderByZIndex(const unordered_map<EntityID, ECS::Entity>& list);
            /**
             * @brief Rounds a coordinate down to its containing logical pixel.
             * @param value Value to convert, store or inspect
             */
            static inline float SnapToPixel(float value) { return std::floor(value); }
            /**
             * @brief Subtracts separately pixel-snapped world and camera coordinates.
             * @param world World coordinate
             * @param camera Camera entity or coordinate used by this operation
             */
            static inline float ToScreen(float world, float camera) {
                return SnapToPixel(world) - SnapToPixel(camera);
            }
    };
}
