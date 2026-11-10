#include <render/gkc_text.h>
#include <algorithm>
#include <render/gkc_font.h>
#include <core/gkc_error.h>
#include <core/gkc_logger.h>

using std::string;

using namespace Galaktic::Render;

Text::Text(TTF_Font* font, const string& text, SDL_Color color)
{
    m_engine = TTF_CreateSurfaceTextEngine();
    if (m_engine != nullptr)
        m_text = TTF_CreateText(m_engine, font, text.c_str(), text.size());
    if (m_text == nullptr || !TTF_SetTextColor(m_text, GKC_SET_COLOR(color)) || !RefreshSurface())
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError, "Failed to create text label!");
    }
}

Text::~Text()
{
    SDL_DestroySurface(m_textSurface);
    TTF_DestroyText(m_text);
    TTF_DestroySurfaceTextEngine(m_engine);
}

bool Text::RefreshSurface()
{
    int width = 0, height = 0, x = 0, y = 0;
    if (!m_text || !TTF_GetTextSize(m_text, &width, &height) || !TTF_GetTextPosition(m_text, &x, &y))
        return false;
    SDL_Surface* surface = SDL_CreateSurface(std::max(1, width), std::max(1, height), SDL_PIXELFORMAT_RGBA32);
    if (!surface) return false;
    SDL_FillSurfaceRect(surface, nullptr, 0);
    if (!TTF_DrawSurfaceText(m_text, -x, -y, surface)) {
        SDL_DestroySurface(surface);
        return false;
    }
    SDL_DestroySurface(m_textSurface);
    m_textSurface = surface;
    return true;
}

void Text::SetColor(SDL_Color color)
{
    if (m_text && TTF_SetTextColor(m_text, GKC_SET_COLOR(color))) RefreshSurface();
}

void Text::SetTextDirection(FontDirection direction)
{
    if (m_text && TTF_SetTextDirection(m_text, FontDirectionToTTF_Direction(direction))) RefreshSurface();
}

void Text::SetFont(TTF_Font* font)
{
    if (m_text && TTF_SetTextFont(m_text, font)) RefreshSurface();
}

void Text::SetPosition(int x, int y)
{
    if (m_text) TTF_SetTextPosition(m_text, x, y);
}

void Text::SetText(const string& text)
{
    if (m_text && TTF_SetTextString(m_text, text.c_str(), text.size())) RefreshSurface();
}
