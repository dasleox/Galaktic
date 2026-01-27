#include <core/gkc_debugger.h>
#include "core/gkc_exception.h"
#include "core/gkc_logger.h"

using namespace Galaktic::Debug;

DebugInformation* Console::m_info = new DebugInformation();
SDL_Renderer* Console::m_renderer = nullptr;
bool Console::m_isActive = false;

DebugInformation::DebugInformation()
    : ram_usage_(0),
      ram_available_(GetSystemRAM()),
      fps_(0.0f),
      x_coordinate_(0.0f),
      y_coordinate_(0.0f)
{
    std::memset(engine_name_, 0, sizeof(engine_name_));
    std::memset(display_info_, 0, sizeof(display_info_));
}

void Console::CallConsole() {
    if (m_renderer == nullptr)
        GKC_ENGINE_ERROR("Renderer could not be initialized.");
    SDL_SetRenderDrawColor(m_renderer, GKC_SET_COLOR(BLACK_COLOR));
    SDL_RenderDebugText(m_renderer, 32.f, 32.f, m_info->engine_name_);
    SDL_RenderDebugText(m_renderer, 32.f, 64.f, m_info->display_info_);
    SDL_RenderDebugTextFormat(m_renderer, 32.f, 96.f, "RAM Usage: %llu", m_info->ram_usage_);
    SDL_RenderDebugTextFormat(m_renderer, 140.f, 96.f, "/ %llu MB", m_info->ram_available_);
    SDL_RenderDebugTextFormat(m_renderer, 32.f, 128.f, "FPS: %f", m_info->fps_);
    SDL_RenderDebugTextFormat(m_renderer, 32.f, 160.f, "X: %.2f", m_info->x_coordinate_);
    SDL_RenderDebugTextFormat(m_renderer, 128.f, 160.f, "Y: %.2f", m_info->y_coordinate_);
}

void Console::CallSimpleConsole(){
    constexpr int WIDTH = 4;

    auto printLine = [&] (const string& str, auto value)
    {
        std::cout << '\r' << str
                  << std::setw(WIDTH)
                  << std::setfill('0')
                  << value
                  << '\n';
    };

    std::cout << '\r' << "RAM: "
              << std::setw(WIDTH) << std::setfill('0') << m_info->ram_usage_
              << '/'
              << std::setw(WIDTH) << std::setfill('0') << m_info->ram_available_ << "MB"
              << '\n';

    printLine("FPS: ",m_info->fps_);
    printLine("X: ", m_info->x_coordinate_);
    printLine("Y: ", m_info->y_coordinate_);

    std::cout.flush();
}

void Galaktic::Debug::StartLibraries() {
    Logger::Init();

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) || !TTF_Init()
        || !MIX_Init()) {
        GKC_THROW_EXCEPTION(GalakticException, "SDL could not initialize!");
    }
    GKC_ENGINE_INFO("Initialized SDL successfully.");
}

Galaktic::Core::DeviceInformation Galaktic::Debug::GetDeviceInformation() {
    Core::DeviceInformation info;
    info.os_ = GKC_OS;
    info.arch_ = GKC_ARCH;
    info.width_ = 800;
    info.height_ = 600;

    SDL_DisplayID display_id = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display_id);

    GKC_ASSERT(display_id >= 1, "Failed to get the primary display!");
    GKC_ASSERT(mode != nullptr, "Failed to get the display mode!");

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Display ID: {0}", display_id);
        GKC_ENGINE_INFO("Current Display Mode: {0}x{1} @ {2}Hz", mode->w, mode->h, mode->refresh_rate);
    #endif

    info.width_ = mode->w;
    info.height_ = mode->h;

    return info;
}
