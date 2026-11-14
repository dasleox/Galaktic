#include <core/managers/gkc_font_man.h>
#include <render/gkc_font.h>
#include <config/gkc_config_values.h>

using std::filesystem::path;
using std::string;
using std::vector;

using namespace Galaktic::Core::Managers;

static const vector<string> FONT_EXTENSIONS = {".ttf"};

FontManager::FontManager(const path& folder)
{
    SetBackupFont();
    
    auto files = Filesystem::GetFilenamesInFolder(folder);
    for (auto& file : files) {
        if (Filesystem::CheckExtension(file, FONT_EXTENSIONS))
            RegisterAsset(file);
    }
}

void FontManager::SetBackupFont()
{
    m_defaultFont = nullptr;
    auto configured = Galaktic::Config::GetValueOrDefault<std::string>(
        "general.cfg", "fallback_font_path", "");
    if(!configured.empty() && configured.find('\0') == std::string::npos) {
        const auto filepath = ConfigManager::GetProjectFolder() / configured;
        m_defaultFont = TTF_OpenFont(filepath.string().c_str(), 12.f);
    }
    if(m_defaultFont == nullptr)
        m_defaultFont = TTF_OpenFont(GKC_ARIAL_FONT.string().c_str(), 12.f);
    if(m_defaultFont == nullptr)
        GKC_ENGINE_WARNING("No usable fallback font is available: {}", SDL_GetError());
}

