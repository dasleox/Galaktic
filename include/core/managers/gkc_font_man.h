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
#include <render/gkc_font.h>

namespace Galaktic::Core::Managers
{
    /**
     * @class FontManager
     * @brief Registers font assets and provides the engine's fallback font.
     */
    class FontManager : public BaseManager<Render::Font, Render::FontInfo, Render::Font_List,
        MAX_FONT_QUANTITY, true>
    {
        public:
            /**
             * @brief Initializes this manager using the supplied project context.
             * @param folder Folder containing the managed files
             */
            explicit FontManager(const std::filesystem::path& folder);
            /**
             * @brief Returns the borrowed engine fallback font.
             */
            static TTF_Font* GetDefaultFont() { return m_defaultFont; }
        private:
            /**
             * @brief Opens the engine fallback font for console and default text rendering.
             */
            static void SetBackupFont();
            static inline TTF_Font* m_defaultFont = nullptr;
    };
}