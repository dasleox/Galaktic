#include <render/gkc_rpixels.h>
#include <core/managers/gkc_ecs_man.h>
#include <core/managers/gkc_texture_man.h>
#include <render/gkc_texture.h>

using namespace Galaktic;
using namespace Galaktic::Render;
using namespace Galaktic::Core;

PixelColoredGroupInfo::PixelColoredGroupInfo(const string& name, SDL_Color mainColor, EntityID id,
    Core::Managers::ECS_Manager& ecsManager) 
    : m_groupName(name), m_mainColor(mainColor), m_ecsManager(ecsManager)
{
    auto& textureComp = ecsManager.GetComponentOfEntity<ECS::TextureComponent>(id);
    auto texture = Managers::TextureManager::GetAssetByID(textureComp.id);

    if(texture == nullptr) {
        GKC_ENGINE_ERROR("Failed to create a pixel colored group for entity '{}'", ecsManager.GetEntityNameByID(id));
        return;
    }
    
    SDL_Texture* SDLtexture = texture->GetSDLTexture();
    if(SDLtexture == nullptr) {
        GKC_ENGINE_ERROR("Failed to create a pixel colored group for entity '{}'", ecsManager.GetEntityNameByID(id));
        return;
    }

    float width = 0, height = 0;
    SDL_GetTextureSize(SDLtexture, &width, &height);

    #if GKC_DEBUG
        GKC_ENGINE_INFO("RPixelGroup '{}', Width: {} | Height: {}", name, width, height);
    #endif

    m_affectedPixels.resize(width);
    m_affectedEntities.emplace(id, id);
}

PixelColoredGroupInfo::~PixelColoredGroupInfo() {}

void PixelColoredGroupInfo::SetSelectedPixels(const PixelMapping_List& list) {
    size_t width = list.size();
    size_t height = list.at(0).size();

    if(width != height) {
        GKC_ENGINE_ERROR("PixelMappingList passed has different width and height!");
        return;
    }

    m_affectedPixels.resize(width);

    for(auto& pixelColumn: m_affectedPixels) {
        pixelColumn.resize(height);
    }

    // Add values to the member list;
    for(uint32_t i = 0; i < width; i++) {
        for(uint32_t j = 0; j < height; j++) {
            m_affectedPixels[i][j] = list[i][j];
        } 
    }
}

void PixelColoredGroupInfo::SetName(const string& name) {
    m_groupName = name;
}

void PixelColoredGroupInfo::SetColor(SDL_Color color) {
    m_mainColor = color;
}

void PixelColoredGroupInfo::AddEntityToGroup(EntityID id) {
    if(!m_ecsManager.EntityExists(id)) {
        GKC_ENGINE_ERROR("Entity to add to this group '{}' doesn't exists!", m_groupName);
        return;
    }
    if(!HasSameTexture(id)) {
        GKC_ENGINE_ERROR("Entity doesn't have the same texture as the parent of the group '{}'!", m_groupName);
        return;
    }

    m_affectedEntities.emplace(id, id);
}

void PixelColoredGroupInfo::DeleteEntityFromGroup(EntityID id) {
    m_affectedEntities.erase(id);
}

bool PixelColoredGroupInfo::HasSameTexture(EntityID id) {
    // Get the texture comp of the first element in the list
    auto& parentTextureComp = m_ecsManager.GetComponentOfEntity<ECS::TextureComponent>(2);
    auto& textureComp = m_ecsManager.GetComponentOfEntity<ECS::TextureComponent>(id);
    if(textureComp.id != parentTextureComp.id)
        return false;
    return true;
}

bool PixelColoredGroupInfo::HasSameWidthAndHeight() {
    return m_affectedPixels.size() == m_affectedPixels.at(0).size();
}

void PixelColoredGroupInfo::ApplyColorToEntities() {
    if(m_affectedPixels.empty() || m_affectedPixels.at(0).empty()) {
        GKC_ENGINE_ERROR("No pixel mapping set! Call SetSelectedPixels first.");
        return;
    }

    size_t textureWidth = m_affectedPixels.size();
    size_t textureHeight = m_affectedPixels.at(0).size();
    
    if(!HasSameWidthAndHeight()) {
        return;
    }

    for(auto& [key, id] : m_affectedEntities) {
        auto& textureComp = m_ecsManager.GetComponentOfEntity<ECS::TextureComponent>(id);
        auto texture = Managers::TextureManager::GetAssetByID(textureComp.id);
        SDL_Texture* SDLtexture = texture->GetSDLTexture();
        SDL_Surface* originalSurface = texture->GetSDLSurface();

        void* pixels;
        int pitch;

        SDL_LockTexture(SDLtexture, NULL, &pixels, &pitch);

        if(pixels == nullptr) {
            GKC_ENGINE_ERROR("Failed to get pixels from texture! (ID: {})", id);
            SDL_UnlockTexture(SDLtexture);
            continue;
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

        for(uint32_t i = 0; i < textureHeight; i++) {
            for(uint32_t j = 0; j < textureWidth; j++) {
                if(m_affectedPixels[i][j] == true) {
                    pixelArray[i * stride + j] = SDL_MapRGBA( SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888),
                        NULL, GKC_SET_COLOR(m_mainColor)
                    );
                }
            }
        }

        SDL_UnlockTexture(SDLtexture);
    }
}