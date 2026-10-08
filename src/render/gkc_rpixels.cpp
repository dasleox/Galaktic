/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <render/gkc_rpixels.h>
#include <core/managers/gkc_ecs_man.h>
#include <core/managers/gkc_texture_man.h>
#include <render/gkc_texture.h>

using namespace Galaktic;
using namespace Galaktic::Render;
using namespace Galaktic::Core;

PixelColoredGroupInfo::PixelColoredGroupInfo(const string& name, SDL_Color mainColor, EntityID id,
    Core::Managers::ECS_Manager& ecsManager)
    : m_groupName(name), m_mainColor(mainColor), m_parentEntityID(id), m_ecsManager(ecsManager)
{
    ECS::Entity* parentEntity = m_ecsManager.GetEntityByID(id);
    if (parentEntity == nullptr || !parentEntity->Has<ECS::TextureComponent>()) {
        GKC_ENGINE_ERROR("Failed to create pixel group '{}': parent entity has no texture!", name);
        return;
    }

    auto& textureComp = parentEntity->Get<ECS::TextureComponent>();
    auto* texture = Managers::TextureManager::GetAssetByID(textureComp.id);
    if (texture == nullptr) {
        GKC_ENGINE_ERROR("Failed to create a pixel colored group for entity '{}'", m_ecsManager.GetEntityNameByID(id));
        return;
    }

    SDL_Texture* sdlTexture = texture->GetSDLTexture();
    if (sdlTexture == nullptr) {
        GKC_ENGINE_ERROR("Failed to create a pixel colored group for entity '{}'", m_ecsManager.GetEntityNameByID(id));
        return;
    }

    float width = 0.f, height = 0.f;
    SDL_GetTextureSize(sdlTexture, &width, &height);
    m_textureWidth  = static_cast<size_t>(width);
    m_textureHeight = static_cast<size_t>(height);

    #if GKC_DEBUG
        GKC_ENGINE_INFO("RPixelGroup '{}', Width: {} | Height: {}", name, m_textureWidth, m_textureHeight);
    #endif

    // Start with nothing selected; every row must exist ahead of time (both
    // dimensions sized), otherwise SetSelectedPixels/ApplyColorToEntities
    // would be indexing rows that were never allocated.
    m_affectedPixels.assign(m_textureHeight, vector<bool>(m_textureWidth, false));
    m_affectedEntities.insert(id);
}

void PixelColoredGroupInfo::SetSelectedPixels(const PixelMapping_List& list) {
    bool widthMatches = list.empty() || list[0].size() == m_textureWidth;
    if (list.size() != m_textureHeight || !widthMatches) {
        GKC_ENGINE_ERROR("Pixel mapping passed to group '{}' doesn't match its texture's dimensions!", m_groupName);
        return;
    }

    m_affectedPixels = list;
}

void PixelColoredGroupInfo::AddEntityToGroup(EntityID id) {
    if (!m_ecsManager.EntityExists(id)) {
        GKC_ENGINE_ERROR("Entity to add to this group '{}' doesn't exists!", m_groupName);
        return;
    }
    if (!HasSameTexture(id)) {
        GKC_ENGINE_ERROR("Entity doesn't have the same texture as the parent of the group '{}'!", m_groupName);
        return;
    }

    m_affectedEntities.insert(id);
}

void PixelColoredGroupInfo::DeleteEntityFromGroup(EntityID id) {
    m_affectedEntities.erase(id);
}

bool PixelColoredGroupInfo::HasSameTexture(EntityID id) const {
    // Compare against the group's actual parent entity instead of an
    // assumed, hardcoded ID - the parent is not always entity #2.
    auto& parentTextureComp = m_ecsManager.GetComponentOfEntity<ECS::TextureComponent>(m_parentEntityID);
    auto& textureComp = m_ecsManager.GetComponentOfEntity<ECS::TextureComponent>(id);
    return textureComp.id == parentTextureComp.id;
}

bool PixelColoredGroupInfo::MatchesTextureDimensions() const {
    return m_affectedPixels.size() == m_textureHeight
        && (m_affectedPixels.empty() || m_affectedPixels[0].size() == m_textureWidth);
}

void PixelColoredGroupInfo::ApplyColorToEntities() {
    if (m_affectedPixels.empty() || m_affectedPixels[0].empty()) {
        GKC_ENGINE_ERROR("No pixel mapping set for group '{}'! Call SetSelectedPixels first.", m_groupName);
        return;
    }

    if (!MatchesTextureDimensions()) {
        GKC_ENGINE_ERROR("Pixel mapping for group '{}' no longer matches its texture!", m_groupName);
        return;
    }

    for (EntityID id : m_affectedEntities) {
        RecolorEntityTexture(id);
    }
}

void PixelColoredGroupInfo::RecolorEntityTexture(EntityID id) {
    auto& textureComp = m_ecsManager.GetComponentOfEntity<ECS::TextureComponent>(id);
    auto* texture = Managers::TextureManager::GetAssetByID(textureComp.id);
    if (texture == nullptr) {
        GKC_ENGINE_ERROR("Entity (ID: {}) in group '{}' has no valid texture!", id, m_groupName);
        return;
    }

    SDL_Texture* sdlTexture = texture->GetSDLTexture();
    SDL_Surface* originalSurface = texture->GetSDLSurface();

    void* pixels = nullptr;
    int pitch = 0;
    if (!SDL_LockTexture(sdlTexture, NULL, &pixels, &pitch) || pixels == nullptr) {
        GKC_ENGINE_ERROR("Failed to get pixels from texture! (ID: {})", id);
        return;
    }

    SDL_ConvertPixels(
        originalSurface->w, originalSurface->h,
        originalSurface->format,
        originalSurface->pixels, originalSurface->pitch,
        originalSurface->format,
        pixels, pitch
    );

    Uint32* pixelArray = static_cast<Uint32*>(pixels);
    int stride = pitch / sizeof(Uint32);

    // Rows (y) first, columns (x) second - this now matches how
    // m_affectedPixels is actually sized (height rows of width columns),
    // instead of transposing width/height like the previous version did.
    for (size_t y = 0; y < m_textureHeight; y++) {
        for (size_t x = 0; x < m_textureWidth; x++) {
            if (m_affectedPixels[y][x]) {
                pixelArray[y * stride + x] = SDL_MapRGBA(
                    SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888),
                    NULL, GKC_SET_COLOR(m_mainColor)
                );
            }
        }
    }

    SDL_UnlockTexture(sdlTexture);
}