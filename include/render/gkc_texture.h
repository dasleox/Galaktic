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

namespace Galaktic::Render 
{
    /**
     * @class Texture
     * @brief Texture used in objects, UI, etc.
     * 
     * This class encapsulates an SDL_Texture and provides methods to manage and access the texture.
     * A path has to be provided to load the texture from a file, and open the SDL_Texture using the provided SDL_Renderer.
     * If the path doesn't exist or the texture fails to load, an error is logged and \b HAS to be checked
     * in a function using a Texture object, to avoid undefined behavior. Use \c IsValid() for that.
     */
    class Texture 
    {
        private:
            SDL_Texture* m_texture = nullptr;
            SDL_Surface* m_surface = nullptr;
        public:
            Texture(const path& path, SDL_Renderer* renderer);
            ~Texture();
            [[nodiscard]] SDL_Texture* GetSDLTexture() const { 
                if(IsValid()) 
                    return m_texture; 
                return nullptr;
            }
            [[nodiscard]] SDL_Surface* GetSDLSurface() const { 
                if(IsSurfaceValid()) 
                    return m_surface; 
                return nullptr; 
            }
            
            bool IsValid() const { return m_texture != nullptr; }
            bool IsSurfaceValid() const { return m_surface != nullptr; }
    };

    /**
     * A struct containing an unique ID for the texture and a unique pointer to the texture itself.
     */
    struct TextureInfo : public AssetInfo<Texture> 
    {
        using AssetInfo<Texture>::AssetInfo;
    };

    typedef unordered_map<uint32_t, unique_ptr<TextureInfo>> Texture_List;
}

/**
 * Helper macro to get the SDL_Texture from a Texture pointer 
 * @param tex Texture pointer
 */
#define GKC_GET_SDLTEXTURE(tex) tex->GetSDLTexture()