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
    /**
     * @class Texture
     * @brief Texture used in objects, UI, etc.
     * 
     * This class encapsulates an SDL_Texture and provides methods to manage and access the texture.
     * A path has to be provided to load the texture from a file, and open the SDL_Texture using the provided SDL_Renderer.
     * If the path doesn't exist or the texture fails to load, an error is logged and \b HAS to be checked
     * in a function using a Texture object, to avoid undefined behavior. Use \c IsValid() for that.
     */
    class Texture {
        public:
            Texture(const path& path, SDL_Renderer* renderer);
            [[nodiscard]] SDL_Texture* GetSDLTexture() const { return m_texture; }
            bool IsValid() const { return m_texture != nullptr; }

            TextureID m_ID = 1;
        private:
            SDL_Texture* m_texture = nullptr;
    };

    /**
     * A struct containing an unique ID for the texture and a unique pointer to the texture itself.
     */
    struct TextureInfo {
        TextureID id_;
        unique_ptr<Texture> texture_;
    };

    typedef unordered_map<string, unique_ptr<TextureInfo>> Texture_List;

    /**
     * @brief Checks if the file is an image file
     * @param path filepath
     * @return true if the file is an image file, false otherwise
     */
    extern bool CheckTextureExtension(const path& path);
}

/**
 * Helper macro to get the SDL_Texture from a Texture pointer 
 * @param tex Texture pointer
 */
#define GKC_GET_SDLTEXTURE(tex) tex->GetSDLTexture()