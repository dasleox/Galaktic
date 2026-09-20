#include <render/gkc_font.h>
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic::Render;

TTF_Direction Galaktic::Render::FontDirectionToTTF_Direction(FontDirection direction)
{
    switch (direction)
    {
        case FontDirection::LeftToRight: return TTF_DIRECTION_LTR;
        case FontDirection::RightToLeft: return TTF_DIRECTION_RTL;
        case FontDirection::TopToBottom: return TTF_DIRECTION_TTB;
        case FontDirection::BottomToTop: return TTF_DIRECTION_BTT;
        default:                         return TTF_DIRECTION_INVALID;
    }
}

TTF_FontStyleFlags Galaktic::Render::FontStyleToTTF_FontStyle(FontStyle style)
{
    switch (style)
    {
        case FontStyle::Normal:        return TTF_STYLE_NORMAL;
        case FontStyle::Bold:          return TTF_STYLE_BOLD;
        case FontStyle::Italic:        return TTF_STYLE_ITALIC;
        case FontStyle::Underline:     return TTF_STYLE_UNDERLINE;
        case FontStyle::StrikeThrough: return TTF_STYLE_STRIKETHROUGH;
        default:                       return TTF_STYLE_NORMAL;
    }
}

TTF_HorizontalAlignment Galaktic::Render::FontAlignmentToTTF_HorizontalAlignment(FontAlignment alignment)
{
    switch (alignment)
    {
        case FontAlignment::Invalid: return TTF_HORIZONTAL_ALIGN_INVALID;
        case FontAlignment::Left:    return TTF_HORIZONTAL_ALIGN_LEFT;
        case FontAlignment::Center:  return TTF_HORIZONTAL_ALIGN_CENTER;
        case FontAlignment::Right:   return TTF_HORIZONTAL_ALIGN_RIGHT;
        default:                     return TTF_HORIZONTAL_ALIGN_INVALID;
    }
}

Font::Font(const path &path, SDL_Renderer* renderer, float size) {
    GKC_ENGINE_INFO("Loading {0}...", path.string());
    if (path.empty() || !Filesystem::CheckFile(path)) {
        GKC_ENGINE_ERROR( "given path doesn't exists!");
        return;
    }
    
    if(size <= 0)
    {
        size = 12.f;
    }

    m_fontSize = size;
    m_font = TTF_OpenFont(path.string().c_str(), size);

    if(m_font == nullptr)
    {
        GKC_ENGINE_ERROR("Failed to open font: '{}'", path.extension().string());
    }
}

Font::~Font() {
    if (m_font != nullptr) {
        m_font = nullptr;
    }
}

void Font::SetFontDirection(FontDirection direction)
{
    TTF_SetFontDirection(m_font, FontDirectionToTTF_Direction(direction));
}

void Font::SetOutline(int outline)
{
    TTF_SetFontOutline(m_font, outline);
}

void Font::SetSize(float fontSize)
{
    TTF_SetFontSize(m_font, fontSize);
}

void Font::SetAlignment(FontAlignment alignment)
{
    TTF_SetFontWrapAlignment(m_font, FontAlignmentToTTF_HorizontalAlignment(alignment));
}