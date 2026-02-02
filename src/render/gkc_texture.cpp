#include <render/gkc_texture.h>
#include "core/gkc_exception.h"
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic::Render;

Texture::Texture(const path &path, SDL_Renderer* renderer) {
    GKC_ENGINE_INFO("Loading {0}...", path.string());
    if (path.empty() || !Filesystem::CheckFile(path)) {
        GKC_ENGINE_ERROR( "given path doesn't exists!");
    }

    m_texture = IMG_LoadTexture(renderer, path.string().c_str());
    if (m_texture == nullptr) {
        GKC_ENGINE_ERROR("failed to load texture!");
    }
}

bool Galaktic::Render::CheckTextureExtension(const path &path) {
    if (path.empty() || !Filesystem::CheckFile(path)) {
        return false;
    }
    auto pathStr = path.extension().string();

    if (pathStr == ".png" || pathStr == ".jpg" || pathStr == ".jpeg" || pathStr == ".webp"
        || pathStr == ".bmp" || pathStr == ".gif") { return true; }

    return false;
}