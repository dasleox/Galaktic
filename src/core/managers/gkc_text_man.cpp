#include <core/managers/gkc_text_man.h>
#include <render/gkc_text.h>
#include <core/gkc_error.h>
#include <core/gkc_logger.h>

using namespace Galaktic::Core::Managers;

void TextManager::StartTextEngine(SDL_Renderer* renderer)
{
    m_textEngine = TTF_CreateRendererTextEngine(renderer);
    if(m_textEngine == nullptr)
    {
        Debug::Logger::LogFatalWithType(ErrorType::DependencyNotFound, "Failed to create a text engine!");
    }
}