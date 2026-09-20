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
    typedef vector<vector<bool>> PixelMapping_List;

    class PixelColoredGroupInfo {
        public:
            PixelColoredGroupInfo(const string& name, SDL_Color mainColor, EntityID id, 
                Core::Managers::ECS_Manager& ecsManager);

            ~PixelColoredGroupInfo();

            void SetSelectedPixels(const PixelMapping_List& list);
            void SetName(const string& newName);
            void SetColor(SDL_Color newColor);
            void AddEntityToGroup(EntityID id);
            void DeleteEntityFromGroup(EntityID id);
            void ApplyColorToEntities();

            string m_groupName;
            SDL_Color m_mainColor;
            unordered_map<EntityID, EntityID> m_affectedEntities;
            PixelMapping_List m_affectedPixels;
        protected:
            bool HasSameTexture(EntityID id);
            bool HasSameWidthAndHeight();
            Core::Managers::ECS_Manager& m_ecsManager;
    };
}