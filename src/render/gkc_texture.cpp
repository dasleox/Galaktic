#include <render/gkc_texture.h>

#include "core/gkc_exception.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic::Render;

Texture::Texture(const path &path, SDL_Renderer* renderer) {
    if (path.empty() || !Filesystem::CheckFile(path)) {
        GKC_THROW_EXCEPTION(Debug::RenderException, "given path doesn't exists!");
    }
    m_texture = IMG_LoadTexture(renderer, path.string().c_str());
    if (m_texture == nullptr) {
        GKC_THROW_EXCEPTION(Debug::RenderException, "failed to load texture!");
    }
}
