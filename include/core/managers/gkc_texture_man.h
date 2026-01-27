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

typedef Uint32 TextureID;
namespace Galaktic::Render {
    struct TextureInfo;
    class Texture;
    typedef unordered_map<string, unique_ptr<TextureInfo>> Texture_List;
}

namespace Galaktic::Core::Managers {

    /**
    * @class TextureManager
    * @brief Manages all textures inside a specified folder
    * @note This class manages all textures inside a folder recursively
    * @note Multiples instances are allowed :)
    */
    class TextureManager {
        public:
            /**
             * @param path folder path
             * @param renderer SDL_Renderer
             */
            TextureManager(const path& path, SDL_Renderer* renderer);

            /**
             * @brief Adds a texture to this instance's list
             * @param path path to the texture (relative or full)
             * @param renderer SDL_Renderer
             */
            void AddTexture(const path &path, SDL_Renderer *renderer);

            /**
             * @brief Deletes the texture on the list
             * @param name The name of the texture
             * @note The input SHOULD be the key of the list, not the path as seen here,
             *       this is a string not a path ;)
             */
            void DeleteTexture(const string &name);

            /**
             * @brief Gets a pointer to the texture in the list
             * @param name The name of the texture
             * @return Pointer to the Texture
             * @note The input SHOULD be the key of the list, not the path as seen here
             *       this is a string not a path ;)
             */
            Render::Texture* GetTexture(const string &name);
            Render::Texture_List& GetTextureList() { return m_textureList; }

            void PrintList() const;
        private:
            Render::Texture_List m_textureList;
    };
}