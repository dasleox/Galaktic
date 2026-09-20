#include <render/gkc_text.h>
#include <render/gkc_font.h>
#include <core/gkc_logger.h>

using namespace Galaktic::Render;

Text::Text(TTF_Font* font, const string& text, SDL_Color color)
{
    m_textSurface = TTF_RenderText_Solid(font, text.c_str(), 0, color);
    if(m_textSurface == nullptr)
    {
        GKC_ENGINE_ERROR("Failed to create text label!");
    }
}

void Text::SetColor(SDL_Color color)
{
    TTF_SetTextColor(m_text, GKC_SET_COLOR(color));
}

void Text::SetTextDirection(FontDirection direction)
{
    TTF_SetTextDirection(m_text, FontDirectionToTTF_Direction(direction));
}

void Text::SetFont(TTF_Font* font)
{
    TTF_SetTextFont(m_text, font);
}

void Text::SetPosition(int x, int y)
{
    TTF_SetTextPosition(m_text, x, y);
}

void Text::SetText(const string& text)
{
    TTF_SetTextString(m_text, text.c_str(), 0);
}