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

    class Text 
    {
        public:
            Text(TTF_Font* font, const string& text, SDL_Color color);

            void SetColor(SDL_Color color);
            void SetTextDirection(FontDirection direction);
            void SetFont(TTF_Font* font);
            void SetPosition(int x, int y);
            void SetText(const string& text);

            TTF_Text* GetText() { return m_text; }
            SDL_Surface* GetTextSurface() { return m_textSurface; }
            bool IsValid() { return m_textSurface != nullptr; }
        private:
            TTF_Text* m_text = nullptr;
            SDL_Surface* m_textSurface = nullptr;
    };

    struct TextInfo : public AssetInfo<Text> 
    {
        using AssetInfo<Text>::AssetInfo;
    };

    typedef unordered_map<uint32_t, unique_ptr<TextInfo>> Text_List;
}