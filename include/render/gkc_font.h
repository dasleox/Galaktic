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
     * @enum FontDirection
     * @brief Identifies the writing direction used for font shaping.
     */
    enum class FontDirection
    {
        Invalid     = TTF_DIRECTION_INVALID,        
        LeftToRight = TTF_DIRECTION_LTR,            /**< Left to Right */
        RightToLeft = TTF_DIRECTION_RTL,            /**< Right to Left */
        TopToBottom = TTF_DIRECTION_TTB,            /**< Top to Bottom */
        BottomToTop = TTF_DIRECTION_BTT             /**< Bottom to Top */
    };

    /**
     * @enum FontStyle
     * @brief Identifies font style flags supported by SDL_ttf.
     */
    enum class FontStyle
    {
        Normal          = TTF_STYLE_NORMAL,
        Bold            = TTF_STYLE_BOLD,         
        Italic          = TTF_STYLE_ITALIC,       
        Underline       = TTF_STYLE_UNDERLINE,     
        StrikeThrough   = TTF_STYLE_STRIKETHROUGH
    };

    /**
     * @enum FontAlignment
     * @brief Identifies horizontal alignment for wrapped font rendering.
     */
    enum class FontAlignment
    {
        Invalid = TTF_HORIZONTAL_ALIGN_INVALID,
        Left    = TTF_HORIZONTAL_ALIGN_LEFT,
        Center  = TTF_HORIZONTAL_ALIGN_CENTER,
        Right   = TTF_HORIZONTAL_ALIGN_RIGHT
    };

    /**
     * @class Font
     * @brief Owns a TTF_Font and exposes font settings. The font must outlive text that borrows it.
     */
    class Font
    {
        public:
            /**
             * @brief Opens a font at the requested size; check IsValid before using it.
             * @param path Source asset filepath
             * @param renderer SDL_Renderer used for drawing; ownership stays with the caller
             * @param size Font size in points
             */
            Font(const std::filesystem::path& path, SDL_Renderer* renderer, float size);
            /**
             * @brief Releases the resources owned by this object.
             */
            ~Font();
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Font(const Font&) = delete;
            /**
             * @brief Disables copying to keep ownership and lifetime tied to one instance.
             */
            Font& operator=(const Font&) = delete;
            /**
             * @brief Returns the borrowed TTF_Font; nullptr if loading failed.
             */
            [[nodiscard]] TTF_Font* GetFont() const { return m_font; }
            /**
             * @brief Returns the last successfully applied font size in points.
             */
            [[nodiscard]] float GetFontSize() const { return m_fontSize; }
            /**
             * @brief Checks whether this handle or underlying resource is initialized for use.
             * @return true if this handle or underlying resource is initialized for use, false otherwise
             */
            bool IsValid() const { return m_font != nullptr; }

            /**
             * @brief Sets the font's shaping direction.
             * @param direction Text shaping direction
             */
            void SetFontDirection(FontDirection direction);
            /**
             * @brief Sets the font outline thickness in pixels.
             * @param outline Font outline thickness in pixels
             */
            void SetOutline(int outline);
            /**
             * @brief Applies a finite positive font size and updates the cached size on success.
             * @param fontSize Font size in points
             */
            void SetSize(float fontSize);
            /**
             * @brief Sets horizontal alignment for wrapped font rendering.
             * @param alignment Horizontal wrapped-text alignment
             */
            void SetAlignment(FontAlignment alignment);

        private:
            TTF_Font* m_font = nullptr;
            float m_fontSize = 12.f;
    };


    /**
     * @brief Converts an engine font direction to the corresponding SDL_ttf value.
     * @param direction Text shaping direction
     */
    extern TTF_Direction FontDirectionToTTF_Direction(FontDirection direction);
    /**
     * @brief Converts an engine font style to the corresponding SDL_ttf flags.
     * @param style Style
     */
    extern TTF_FontStyleFlags FontStyleToTTF_FontStyle(FontStyle style);
    /**
     * @brief Converts an engine font alignment to the corresponding SDL_ttf value.
     * @param alignment Horizontal wrapped-text alignment
     */
    extern TTF_HorizontalAlignment FontAlignmentToTTF_HorizontalAlignment(FontAlignment alignment);
    
    /**
     * A struct containing an unique ID for the texture and a shared pointer to the font itself.
     */
    struct FontInfo : public AssetInfo<Font> 
    {
        using AssetInfo<Font>::AssetInfo;
    };

    typedef std::unordered_map<uint32_t, std::unique_ptr<FontInfo>> Font_List;
}

/**
 * Helper macro to get the SDL_Texture from a Texture pointer 
 * @param font Font pointer
 */
#define GKC_GET_FONT(font) font->GetFont()
