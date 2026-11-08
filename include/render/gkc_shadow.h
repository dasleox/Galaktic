#pragma once

#include <pch.hpp>

namespace Galaktic::ECS {
    // Opt in to casting shadows. Entities without this still receive lighting.
    /**
     * @struct ShadowComponent
     * @brief Opts an entity into casting hard shadows using its alpha cutoff.
     */
    struct ShadowComponent {
        bool castsShadow = true;
        // Binary hard shadows: alpha >= cutoff blocks a light completely.
        // Zero-alpha pixels never block, even if cutoff is set to zero.
        Uint8 alphaCutoff = 1;
    };
}

namespace Galaktic::Render {
    class Drawer;

    namespace Detail {
        /**
         * @struct ShadowAlphaMask
         * @brief Caches row-major source alpha values for the current lighting pass.
         */
        struct ShadowAlphaMask {
            int width = 0;
            int height = 0;
            bool used = false;
            std::vector<Uint8> pixels;
        };

        /**
         * @struct ShadowOccluder
         * @brief Stores one transformed shadow caster and borrows its current alpha mask.
         */
        struct ShadowOccluder {
            EntityID owner = InvalidEntity;
            SDL_FRect rect{};
            SDL_FRect bounds{};
            float cosine = 1.f;
            float sine = 0.f;
            Uint8 alpha = 255;
            Uint8 alphaCutoff = 1;
            // Borrowed from LightingState::masks during DrawLighting only.
            const ShadowAlphaMask* mask = nullptr;
        };

        /**
         * @struct SceneLight
         * @brief Stores a pixel-snapped point light prepared for the current lighting pass.
         */
        struct SceneLight {
            EntityID owner = InvalidEntity;
            int x = 0;
            int y = 0;
            int radius = 0;
            float radiusSquared = 0.f;
            float intensity = 0.f;
            SDL_FColor color{};
        };
    }

    /** Per-scene reusable lighting resources. Destroy before the SDL_Renderer.
     *  Never share a state between renderers. Copying is disabled by unique_ptr.
     */
    struct LightingState {
        /**
         * @brief Creates empty lighting buffers; renderer resources are allocated on the first lighting pass.
         */
        LightingState() = default;

        private:
            friend class Drawer;

            std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture{
                nullptr, &SDL_DestroyTexture};
            SDL_Renderer* renderer = nullptr;
            int width = 0;
            int height = 0;
            std::vector<float> brightness;
            std::vector<Uint8> output;
            std::vector<Uint8> blocked;
            std::vector<Uint8> visible;
            std::vector<Detail::SceneLight> lights;
            std::vector<Detail::ShadowOccluder> occluders;
            std::unordered_map<SDL_Surface*, Detail::ShadowAlphaMask> masks;
    };
}
