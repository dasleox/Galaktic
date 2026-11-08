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
    class Font;
    enum class FontDirection;

    /**
     * @class Text
     * @brief Owns a surface text engine, its text object and cached pixels; borrows the font.
     */
    class Text 
    {
        public:
            /**
             * @brief Creates surface-backed text using a borrowed font and caches its rendered pixels.
             * @param font Borrowed TTF_Font, which must outlive the text using it
             * @param text Text string
             * @param color RGBA color
             */
            Text(TTF_Font* font, const string& text, SDL_Color color);
            /**
             * @brief Releases the resources owned by this object.
             */
            ~Text();
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Text(const Text&) = delete;
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Text& operator=(const Text&) = delete;

            /**
             * @brief Sets the RGBA color used by this object.
             * @param color RGBA color
             */
            void SetColor(SDL_Color color);
            /**
             * @brief Changes the text's shaping direction and refreshes its cached surface.
             * @param direction Text shaping direction
             */
            void SetTextDirection(FontDirection direction);
            /**
             * @brief Changes the text's borrowed font and refreshes its cached surface.
             * @param font Borrowed TTF_Font, which must outlive the text using it
             */
            void SetFont(TTF_Font* font);
            /**
             * @brief Sets the SDL_ttf text object's position; cached glyph pixels remain local to the text.
             * @param x Horizontal coordinate or amount
             * @param y Vertical coordinate or amount
             */
            void SetPosition(int x, int y);
            /**
             * @brief Replaces the text string and refreshes its cached surface.
             * @param text Text string
             */
            void SetText(const string& text);

            /**
             * @brief Returns the borrowed surface-engine TTF_Text owned by this instance.
             */
            TTF_Text* GetText() { return m_text; }
            /**
             * @brief Returns the borrowed cached text surface, valid until the next successful text update or destruction.
             */
            SDL_Surface* GetTextSurface() { return m_textSurface; }
            /**
             * @brief Checks whether this handle or underlying resource is initialized for use.
             * @return true if this handle or underlying resource is initialized for use, false otherwise
             */
            bool IsValid() { return m_textSurface != nullptr; }
        private:
            /**
             * @brief Rebuilds cached RGBA text pixels from the current SDL_ttf text properties.
             * @return true on success, false otherwise
             */
            bool RefreshSurface();
            TTF_TextEngine* m_engine = nullptr;
            TTF_Text* m_text = nullptr;
            SDL_Surface* m_textSurface = nullptr;
    };

    /**
     * @struct TextInfo
     * @brief Stores metadata and unique ownership of a text asset.
     */
    struct TextInfo : public AssetInfo<Text> 
    {
        using AssetInfo<Text>::AssetInfo;
    };

    typedef unordered_map<uint32_t, unique_ptr<TextInfo>> Text_List;
}
