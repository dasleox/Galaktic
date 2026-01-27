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
    /**
     * @class Texture
     * @brief Texture used in objects, UI, etc.
     */
    class Texture {
        public:
            Texture(const path& path, SDL_Renderer* renderer);
            [[nodiscard]] SDL_Texture* GetSDLTexture() const { return m_texture; }

            TextureID m_ID = 1;
        private:
            SDL_Texture* m_texture = nullptr;
    };

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

#define GKC_GET_SDLTEXTURE(tex) tex->GetSDLTexture()