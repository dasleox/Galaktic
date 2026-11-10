#include <core/gkc_console.h>
#include <core/gkc_logger.h>
#include <config/gkc_config.h>
#include <script/gkc_library.h>
#include <core/gkc_scene.h>
#include <core/gkc_clock.h>
#include <core/gkc_debugger.h>
#include <core/events/gkc_dispatcher.h>
#include <core/systems/gkc_key.h>
#include <sstream>

using std::string;
using std::vector;

using namespace Galaktic;
using namespace Galaktic::Core;

constexpr int MAX_ERRORS_ON_SCREEN = 5;
constexpr Uint64 ERROR_DISPLAY_MS = 5000;

void Console::ScrollBy(int lines)
{
    const int total = static_cast<int>(m_sink->GetLines().size());
    const int maxOffset = std::max(0, total - m_visibleLines);
    m_scrollOffset = std::clamp(m_scrollOffset + lines, 0, maxOffset);
}

void Console::DisplayErrors(SDL_Renderer* renderer, TTF_Font* font)
{
    if(!m_sink || font == nullptr)
    {
        return;
    }

    const Uint64 now = SDL_GetTicks();
    const auto& lines = m_sink->GetLines();
    const float lineHeight = (float)TTF_GetFontHeight(font) + 2.f;

    // Lines are in chronological order: walk from the newest and stop at the first old one
    vector<const Debug::ConsoleLine*> recent;
    for(auto it = lines.rbegin(); it != lines.rend() && recent.size() < MAX_ERRORS_ON_SCREEN; ++it)
    {
        if(now - it->timestamp > ERROR_DISPLAY_MS)
        {
            break;
        }
        if(it->level >= spdlog::level::err)
        {
            recent.push_back(&*it);
        }
    }

    // Oldest at the top, newest at the bottom
    float y = 8.f;
    for(auto it = recent.rbegin(); it != recent.rend(); ++it)
    {
        SDL_Surface* surf = TTF_RenderText_Blended(font, CleanLogText((*it)->text).c_str(), 0, RED_COLOR);
        if(!surf)
        {
            continue;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        if(tex)
        {
            SDL_FRect dst = { 8.f, y, (float)surf->w, (float)surf->h };
            SDL_RenderTexture(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_DestroySurface(surf);
        y += lineHeight;
    }
}

void Console::Display(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight) 
{
    if (renderer == nullptr) return;

    int logicalWidth = 0, logicalHeight = 0;
    SDL_RendererLogicalPresentation presentation;
    SDL_Rect viewport{}, clip{};
    float scaleX = 1.f, scaleY = 1.f;
    SDL_BlendMode blendMode;
    SDL_Color drawColor{};
    if (!SDL_GetRenderLogicalPresentation(renderer, &logicalWidth, &logicalHeight, &presentation) ||
        !SDL_GetRenderViewport(renderer, &viewport) ||
        !SDL_GetRenderClipRect(renderer, &clip) ||
        !SDL_GetRenderScale(renderer, &scaleX, &scaleY) ||
        !SDL_GetRenderDrawBlendMode(renderer, &blendMode) ||
        !SDL_GetRenderDrawColor(renderer, &drawColor.r, &drawColor.g, &drawColor.b, &drawColor.a))
        return;
    const bool viewportSet = SDL_RenderViewportSet(renderer);
    const bool clipEnabled = SDL_RenderClipEnabled(renderer);

    // Text and panel both use native output pixels. Restore world coordinates
    // before the caller presents the frame, including all early-return paths
    // inside DisplayContents (closed console, missing font, etc.).
    if (SDL_SetRenderLogicalPresentation(renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED) &&
        SDL_SetRenderScale(renderer, 1.f, 1.f) &&
        SDL_SetRenderViewport(renderer, nullptr) &&
        SDL_SetRenderClipRect(renderer, nullptr) &&
        SDL_GetCurrentRenderOutputSize(renderer, &windowWidth, &windowHeight))
    {
        DisplayContents(renderer, font, windowWidth, windowHeight);
    }

    SDL_SetRenderLogicalPresentation(renderer, logicalWidth, logicalHeight, presentation);
    SDL_SetRenderScale(renderer, scaleX, scaleY);
    SDL_SetRenderViewport(renderer, viewportSet ? &viewport : nullptr);
    SDL_SetRenderClipRect(renderer, clipEnabled ? &clip : nullptr);
    SDL_SetRenderDrawBlendMode(renderer, blendMode);
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
}

void Console::DisplayContents(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight)
{
    if(!m_isVisible)
    {
        DisplayErrors(renderer, font);
        return;
    } 

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_FRect consolePanel = {0, 0, (float) windowWidth, (float) windowHeight * 0.4f};
    SDL_RenderFillRect(renderer, &consolePanel);

    // The panel still shows without a font so toggling the console is
    // visibly doing something even if text rendering isn't available.
    if (font == nullptr)
    {
        return;
    }

    int lineHeight = TTF_GetFontHeight(font) + 4;

    // Bottom row of the panel is reserved for the input line; log lines
    // stack upward from just above it, most recent line closest to input.
    float inputRowY = consolePanel.h - (float)lineHeight;

    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
    SDL_RenderLine(renderer, 0.f, inputRowY, consolePanel.w, inputRowY);

    auto& lines = m_sink->GetLines();
    const int visibleAboveInput = std::max(0, static_cast<int>(inputRowY / lineHeight));
    const int total = static_cast<int>(lines.size());

    const int maxVisible = std::max(1, static_cast<int>(consolePanel.h / lineHeight) - 1);
    m_visibleLines = maxVisible;

    // While scrolled up, keep the same text on screen as new lines arrive
    if (m_scrollOffset > 0 && total > m_lastLineCount) {
        m_scrollOffset += total - m_lastLineCount;
    }

    m_lastLineCount = total;
    m_scrollOffset = std::clamp(m_scrollOffset, 0, std::max(0, total - maxVisible));

    const int newest = total - 1 - m_scrollOffset;                   // bottom-most visible line
    const int oldest = std::max(0, newest - visibleAboveInput + 1);  // top-most visible line

    string promptLine = "> " + m_inputBuffer;
    SDL_Surface* promptSurf = TTF_RenderText_Blended(font, promptLine.c_str(), 0, WHITE_COLOR);
    if (promptSurf != nullptr) {
        SDL_Texture* promptTex = SDL_CreateTextureFromSurface(renderer, promptSurf);
        SDL_FRect promptDst = { 4.f, inputRowY + 1.f, (float)promptSurf->w, (float)promptSurf->h };
        SDL_RenderTexture(renderer, promptTex, NULL, &promptDst);
        SDL_DestroySurface(promptSurf);
        SDL_DestroyTexture(promptTex);
    }


    float y = inputRowY - static_cast<float>(lineHeight);
    for (int i = newest; i >= oldest; --i, y -= lineHeight) {
        SDL_Color color = LevelToColor(lines[i].level);
        SDL_Surface* surf = TTF_RenderText_Blended(font, CleanLogText(lines[i].text).c_str(), 0, color);
        if (!surf) continue;
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FRect dst = { 4.f, y, (float)surf->w, (float)surf->h };
        SDL_RenderTexture(renderer, tex, NULL, &dst);
        SDL_DestroySurface(surf);
        SDL_DestroyTexture(tex);
    }
}

void Console::ShowDebugInformation()
{
    GKC_ENGINE_INFO("{}", Debug::GetEngineName());
}

void Console::ShowFPS()
{
    double dt = Clock::GetDeltaTime();
    double fps = dt > 0.0 ? 1.0 / dt : 0.0;
    GKC_ENGINE_INFO("FPS: {:.1f}", fps);
}

void Console::DisplayHelp(const string& type)
{
    if (type.empty())
    {
        GKC_ENGINE_INFO("Usage: help <section>");
        GKC_ENGINE_INFO(" Sections: scene | entity | script | app | sound | debug");
        return;
    }

    if (type == "scene")
    {
        GKC_ENGINE_INFO("Scene commands:");
        GKC_ENGINE_INFO(" save | load <file> | show_scene_list | pause");
    }
    else if (type == "entity")
    {
        GKC_ENGINE_INFO("Entity commands:");
        GKC_ENGINE_INFO(" create_static_object <name> | create_physics_object <name>");
        GKC_ENGINE_INFO(" copy_entity <name> [new_name] | delete_entity <name>");
        GKC_ENGINE_INFO(" teleport_entity <name> <x> <y> | set_rotation_entity <name> <deg>");
        GKC_ENGINE_INFO(" get_entity_info <name> | print_entity_list");
    }
    else if (type == "script")
    {
        GKC_ENGINE_INFO("Script commands:");
        GKC_ENGINE_INFO(" execute_script <name> | print_luastack | bind <key> <action>");
    }
    else if (type == "app")
    {
        GKC_ENGINE_INFO("App commands:");
        GKC_ENGINE_INFO(" set_fullscreen | set_cheats <true|false> | clear | help [section]");
    }
    else if (type == "sound")
    {
        GKC_ENGINE_INFO("Sound commands:");
        GKC_ENGINE_INFO(" play_sound <name> | stop_sound <name> | stop_all_sounds | print_sound_list | set_master_volume <volume>");
    }
    else if (type == "debug")
    {
        GKC_ENGINE_INFO("Debug commands:");
        GKC_ENGINE_INFO(" show_fps | show_wireframe | show_colliders | show_debug_info");
    }
    else
    {
        InvalidArgumentError("Unknown help section '" + type + "'");
    }
}
