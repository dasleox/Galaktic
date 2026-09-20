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
    enum class FontDirection
    {
        Invalid     = TTF_DIRECTION_INVALID,        
        LeftToRight = TTF_DIRECTION_LTR,            /**< Left to Right */
        RightToLeft = TTF_DIRECTION_RTL,            /**< Right to Left */
        TopToBottom = TTF_DIRECTION_TTB,            /**< Top to Bottom */
        BottomToTop = TTF_DIRECTION_BTT             /**< Bottom to Top */
    };

    enum class FontStyle
    {
        Normal          = TTF_STYLE_NORMAL,
        Bold            = TTF_STYLE_BOLD,         
        Italic          = TTF_STYLE_ITALIC,       
        Underline       = TTF_STYLE_UNDERLINE,     
        StrikeThrough   = TTF_STYLE_STRIKETHROUGH
    };

    enum class FontAlignment
    {
        Invalid = TTF_HORIZONTAL_ALIGN_INVALID,
        Left    = TTF_HORIZONTAL_ALIGN_LEFT,
        Center  = TTF_HORIZONTAL_ALIGN_CENTER,
        Right   = TTF_HORIZONTAL_ALIGN_RIGHT
    };

    class Font
    {
        public:
            Font(const path& path, SDL_Renderer* renderer, float size);
            ~Font();
            [[nodiscard]] TTF_Font* GetFont() const { return m_font; }
            [[nodiscard]] float GetFontSize() const { return m_fontSize; }
            bool IsValid() const { return m_font != nullptr; }

            void SetFontDirection(FontDirection direction);
            void SetOutline(int outline);
            void SetSize(float fontSize);
            void SetAlignment(FontAlignment alignment);

        private:
            TTF_Font* m_font = nullptr;
            float m_fontSize;
    };


    extern TTF_Direction FontDirectionToTTF_Direction(FontDirection direction);
    extern TTF_FontStyleFlags FontStyleToTTF_FontStyle(FontStyle style);
    extern TTF_HorizontalAlignment FontAlignmentToTTF_HorizontalAlignment(FontAlignment alignment);
    
    /**
     * A struct containing an unique ID for the texture and a shared pointer to the font itself.
     */
    struct FontInfo : public AssetInfo<Font> 
    {
        using AssetInfo<Font>::AssetInfo;
    };

    typedef unordered_map<uint32_t, unique_ptr<FontInfo>> Font_List;
}

/**
 * Helper macro to get the SDL_Texture from a Texture pointer 
 * @param font Font pointer
 */
#define GKC_GET_FONT(font) font->GetFont()