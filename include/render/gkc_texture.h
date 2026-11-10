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
            /**
             * @brief Loads an image, retains an RGBA32 source surface and creates a nearest-scaled streaming texture.
             * @param path Source asset filepath
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             */
            Texture(const std::filesystem::path& path, SDL_Renderer* renderer);
            /**
             * @brief Releases the resources owned by this object.
             */
            ~Texture();
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Texture(const Texture&) = delete;
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Texture& operator=(const Texture&) = delete;
            /**
             * @brief Returns the borrowed renderer texture; nullptr if loading or upload failed.
             */
            [[nodiscard]] SDL_Texture* GetSDLTexture() const { 
                if(IsValid()) 
                    return m_texture; 
                return nullptr;
            }
            /**
             * @brief Returns the borrowed RGBA32 source surface; its lifetime belongs to the texture asset.
             */
            [[nodiscard]] SDL_Surface* GetSDLSurface() const { 
                if(IsSurfaceValid()) 
                    return m_surface; 
                return nullptr; 
            }
            
            /**
             * @brief Checks whether this handle or underlying resource is initialized for use.
             * @return true if this handle or underlying resource is initialized for use, false otherwise
             */
            bool IsValid() const { return m_texture != nullptr; }
            /**
             * @brief Checks whether the texture retains a CPU source surface.
             * @return true if the texture retains a CPU source surface, false otherwise
             */
            bool IsSurfaceValid() const { return m_surface != nullptr; }
    };

    /**
     * A struct containing an unique ID for the texture and a unique pointer to the texture itself.
     */
    struct TextureInfo : public AssetInfo<Texture> 
    {
        using AssetInfo<Texture>::AssetInfo;
    };

    typedef std::unordered_map<uint32_t, std::unique_ptr<TextureInfo>> Texture_List;
}

/**
 * Helper macro to get the SDL_Texture from a Texture pointer 
 * @param tex Texture pointer
 */
#define GKC_GET_SDLTEXTURE(tex) tex->GetSDLTexture()
