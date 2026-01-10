#include <core/gkc_clock.h>

using namespace Galaktic::Core;

void Clock::Init() {
    m_frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    m_lastCounter = SDL_GetPerformanceCounter();
    m_deltaTime = 0.0;
}

void Clock::Update() {
    Uint64 current = SDL_GetPerformanceCounter();
    m_deltaTime = static_cast<double>(current - m_lastCounter) / m_frequency;
    m_lastCounter = current;
}
