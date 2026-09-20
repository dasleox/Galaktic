#include <core/managers/gkc_font_man.h>
#include <render/gkc_font.h>

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
    m_defaultFont = TTF_OpenFont(GKC_ARIAL_FONT.string().c_str(), 12.f);
    GKC_RELEASE_ASSERT(m_defaultFont != nullptr, "You are fucked man...." );
}

