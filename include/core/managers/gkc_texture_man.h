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
#include <core/managers/gkc_manager.h>
#include <render/gkc_texture.h>
namespace Galaktic::Core::Managers {
    /**
    * @class TextureManager
    * Manages all textures inside a specified folder, all files that are not situable
    * for textures are automatically ignored. The textures are put inside
    * the texture list an can be accessed statically from anywhere in the engine.
    * However, a previous initialization of this manager is required.
    * 
    * When this manager is initialized textures need to be loaded afterwards
    * using \c LoadAllTextures() , individual textures can also be loaded, if they
    * don't exist they will be automatically added to the texture list
    * 
    * Textures are stored in the list with their filename (extension included: e.g. texture.png)
    * as a key and the value stored inside a TextureInfo struct.
    */
    class TextureManager : public BaseManager<Render::Texture, Render::TextureInfo, Render::Texture_List,
        MAX_TEXTURE_QUANTITY, true> 
    {
        public:
            /**
             * @param path folder path
             */
            TextureManager(const string& path);


            static SDL_Texture* GetMissingTexture();

            static void CreateMissingTexture(SDL_Renderer* renderer);
            /**
             * @brief Destroys the missing texture to free memory
             */
            static void DestroyMissingTexture();
        private:
            static SDL_Texture* m_missingTexture;
    };
}