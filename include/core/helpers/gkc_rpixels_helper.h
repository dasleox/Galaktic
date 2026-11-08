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

namespace Galaktic::Render {
    class PixelColoredGroupInfo;
}
namespace Galaktic::Core::Managers {
    class ECS_Manager;
}

namespace Galaktic::Core::Helpers {
    /**
     * @class RPixelsHelper
     * @brief Owns named pixel-color groups associated with an ECS manager.
     */
    class RPixelsHelper {
        public:
            /**
             * @brief Stores the borrowed ECS manager used by this helper.
             * @param manager Borrowed ECS manager that must outlive this helper
             */
            RPixelsHelper(Managers::ECS_Manager& manager);
             /**
              * @brief Creates a named pixel-color group using the entity's texture dimensions.
              * @param groupName Name identifying the pixel or audio group
              * @param id Entity or asset ID used by this operation
              */
             void CreatePixelGroup(const string& groupName, EntityID id);
             /**
              * @brief Removes the named pixel-color group from the helper.
              * @param groupName Name identifying the pixel or audio group
              */
             void DeletePixelGroup(const string& groupName);

             /**
              * @brief Adds an entity to the named group after validating texture compatibility.
              * @param groupName Name identifying the pixel or audio group
              * @param id Entity or asset ID used by this operation
              */
             void AddEntityToGroup(const string& groupName, EntityID id);
             /**
              * @brief Removes an entity from the group and forgets its painted texture.
              * @param groupName Name identifying the pixel or audio group
              * @param id Entity or asset ID used by this operation
              */
             void DeleteEntityFromGroup(const string& groupName, EntityID id);
            
             /**
              * @brief Refreshes changed textures in every owned pixel group.
              */
             void DrawAllGroups();

             /**
              * @brief Returns shared ownership of the named pixel group, or nullptr when unavailable.
              * @param groupName Name identifying the pixel or audio group
              */
             shared_ptr<Render::PixelColoredGroupInfo> GetPixelGroup(const string& groupName);
        private:
             unordered_map<string, shared_ptr<Render::PixelColoredGroupInfo>> m_groupList; 
             Managers::ECS_Manager& m_ecsManager; 
    };
}