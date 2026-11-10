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

namespace Galaktic::Render {
    /**
     * A pixel selection mask, addressed row-major as \c list[y][x]. This matches
     * how the underlying SDL pixel buffer is addressed in
     * \c PixelColoredGroupInfo::RecolorEntityTexture, so a mask built for one
     * texture size can be applied directly without transposing it.
     */
    typedef std::vector<std::vector<bool>> PixelMapping_List;

    /**
     * @class PixelColoredGroupInfo
     * @brief Groups entities that share a texture so a chosen set of pixels can be
     *        recolored across all of them at once.
     *
     * A group is anchored to a "parent" entity (the one passed to the constructor):
     * its texture defines the group's pixel dimensions, and every pixel mask handed
     * to \c SetSelectedPixels is validated against those dimensions. Any entity added
     * afterwards via \c AddEntityToGroup must share that same texture, since the mask
     * only makes sense for one texture layout.
     *
     * @attention This class does not own the texture or the entities in the group,
     * it only reads their existing \c TextureComponent through the \c ECS_Manager
     * reference passed to the constructor.
     */
    class PixelColoredGroupInfo {
        public:
            /**
             * @param name Name of the group (used for logging/identification)
             * @param mainColor Color applied to the selected pixels
             * @param id Entity ID of the group's parent; its texture defines the group's dimensions
             * @param ecsManager ECS_Manager owning the entity/component data
             */
            PixelColoredGroupInfo(const std::string& name, SDL_Color mainColor, EntityID id,
                Core::Managers::ECS_Manager& ecsManager);

            /**
             * @brief Destroys this object; borrowed entities and external resources remain owned by their callers.
             */
            ~PixelColoredGroupInfo() = default;

            /**
             * @brief Sets which pixels of the group's texture are affected by the group's color
             * @param list Pixel mask, addressed as list[y][x]; its dimensions must match
             *        the parent texture's height and width or the call is rejected
             */
            void SetSelectedPixels(const PixelMapping_List& list);

            /**
             * @brief Changes the stored name.
             * @param newName Replacement name
             */
            void SetName(const std::string& newName) { m_groupName = newName; }
            /**
             * @brief Stores a new group color and marks painted textures for refresh.
             * @param newColor New color
             */
            void SetColor(SDL_Color newColor) { m_mainColor = newColor; m_paintedTextures.clear(); }

            /**
             * @brief Returns the name identifying this object or event.
             */
            [[nodiscard]] const std::string& GetName() const { return m_groupName; }
            /**
             * @brief Returns the RGBA color stored by this object.
             */
            [[nodiscard]] SDL_Color GetColor() const { return m_mainColor; }
            /**
             * @brief Returns the width in pixels.
             */
            [[nodiscard]] size_t GetWidth() const { return m_textureWidth; }
            /**
             * @brief Returns the height in pixels.
             */
            [[nodiscard]] size_t GetHeight() const { return m_textureHeight; }
            /**
             * @brief Returns the number of entities stored in this manager or group.
             */
            [[nodiscard]] size_t GetEntityCount() const { return m_affectedEntities.size(); }

            /**
             * @brief Adds an entity to the group
             * @param id Entity's ID; it must exist and share the parent's texture
             */
            void AddEntityToGroup(EntityID id);

            /**
             * @brief Removes an entity from the group
             * @param id Entity's ID
             */
            void DeleteEntityFromGroup(EntityID id);

            /**
             * @brief Recolors the selected pixels on every entity currently in the group
             */
            void ApplyColorToEntities();
            
            /**
             * @brief Reapplies selected pixel colors after group edits or texture replacement.
             */
            void RefreshChangedTextures();
        private:
            std::string m_groupName;
            SDL_Color m_mainColor;
            EntityID m_parentEntityID = InvalidEntity;
            Uint32 m_textureWidth = 0;
            Uint32 m_textureHeight = 0;
            std::unordered_set<EntityID> m_affectedEntities;
            std::unordered_map<EntityID, const SDL_Texture*> m_paintedTextures;
            PixelMapping_List m_affectedPixels;
            Core::Managers::ECS_Manager& m_ecsManager;

            /**
             * @brief Copies source pixels and recolors the selected pixels of a compatible entity texture.
             * @param id Entity or asset ID used by this operation
             */
            void ApplyColorToEntity(EntityID id);
            /**
             * @brief Checks whether the group parent and candidate still exist and reference the same texture asset.
             * @param id Entity or asset ID used by this operation
             * @return true if the group parent and candidate still exist and reference the same texture asset, false otherwise
             */
            bool HasSameTexture(EntityID id) const;
            /**
             * @brief Checks the height and every row width of the selected-pixel mask.
             * @return true on success, false otherwise
             */
            bool MatchesTextureDimensions() const;
    };
}
