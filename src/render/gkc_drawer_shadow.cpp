#include <render/gkc_drawer.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <core/managers/gkc_animation_man.h>
#include <core/managers/gkc_texture_man.h>
#include <core/systems/gkc_camera_system.h>
#include <ecs/gkc_components.h>
#include <ecs/gkc_entity.h>
#include <render/gkc_animation.h>
#include <render/gkc_texture.h>

using namespace Galaktic;
using namespace Galaktic::Render;

bool Drawer::PrepareLighting(LightingState& state, SDL_Renderer* renderer, int width, int height)
{
    if (renderer == nullptr || width <= 0 || height <= 0)
        return SDL_SetError("Invalid lighting renderer or dimensions");
    if (width > std::numeric_limits<int>::max() / 4)
        return SDL_SetError("Lighting texture pitch exceeds SDL limits");
    if (state.renderer != nullptr && state.renderer != renderer)
        return SDL_SetError("LightingState cannot be shared between renderers");

    const size_t w = static_cast<size_t>(width);
    const size_t h = static_cast<size_t>(height);
    if (w > std::numeric_limits<size_t>::max() / h)
        return SDL_SetError("Lighting dimensions exceed addressable memory");
    const size_t count = w * h;
    if (count > state.brightness.max_size() / 3 || count > state.output.max_size() / 4)
        return SDL_SetError("Lighting buffers exceed addressable memory");
    if (state.texture && state.width == width && state.height == height)
        return true;

    std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture{
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING, width, height), &SDL_DestroyTexture};
    if (!texture) return false;
    if (!SDL_SetTextureScaleMode(texture.get(), SDL_SCALEMODE_NEAREST) ||
        !SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_MOD))
        return false;

    state.brightness.resize(count * 3);
    state.output.resize(count * 4);
    state.texture = std::move(texture);
    state.renderer = renderer;
    state.width = width;
    state.height = height;
    return true;
}

bool Drawer::CollectLights(const ECS::Entity_List& list, float cameraX, float cameraY,
    LightingState& state)
{
    state.lights.clear();
    const double snappedCameraX = std::floor(static_cast<double>(cameraX));
    const double snappedCameraY = std::floor(static_cast<double>(cameraY));

    for (auto [id, entity] : list) {
        if (id == InvalidEntity || !entity.Has<ECS::LightComponent>()) continue;
        if (entity.Has<ECS::VisibilityComponent>() &&
            !entity.Get<ECS::VisibilityComponent>().visible) continue;

        const auto& component = entity.Get<ECS::LightComponent>();
        if (!std::isfinite(component.location.x) || !std::isfinite(component.location.y) ||
            !std::isfinite(component.radius) || !std::isfinite(component.watts) ||
            component.radius <= 0.f || component.watts <= 0.f || component.color.a == 0)
            continue;

        const double radiusPixels = static_cast<double>(component.radius) * 32.0;
        const double x = std::floor(static_cast<double>(component.location.x)) - snappedCameraX;
        const double y = std::floor(static_cast<double>(component.location.y)) - snappedCameraY;
        if (x + radiusPixels < 0.0 || y + radiusPixels < 0.0 ||
            x - radiusPixels >= state.width || y - radiusPixels >= state.height)
            continue;
        if (radiusPixels > MaxShadowRadiusPixels)
            return SDL_SetError("Visible light exceeds maximum shadow radius (%d pixels)",
                MaxShadowRadiusPixels);

        constexpr double coordinateLimit = static_cast<double>(std::numeric_limits<int>::max()) -
            MaxShadowRadiusPixels - 1.0;
        if (x < -coordinateLimit || x > coordinateLimit || y < -coordinateLimit || y > coordinateLimit)
            return SDL_SetError("Light coordinates exceed supported limits");

        SceneLight light;
        light.owner = id;
        light.x = static_cast<int>(x);
        light.y = static_cast<int>(y);
        light.radius = static_cast<int>(std::ceil(radiusPixels));
        light.radiusSquared = static_cast<float>(radiusPixels * radiusPixels);
        // Artistic engine convention, not a physical conversion from watts.
        light.intensity = (component.watts / 100.f) * (component.color.a / 255.f);
        light.color = {component.color.r / 255.f, component.color.g / 255.f,
            component.color.b / 255.f, 1.f};
        state.lights.push_back(light);
    }
    return true;
}

const Drawer::AlphaMask* Drawer::GetAlphaMask(SDL_Surface* surface, LightingState& state)
{
    if (surface == nullptr || surface->w <= 0 || surface->h <= 0) {
        SDL_SetError("Invalid shadow source surface");
        return nullptr;
    }
    auto& mask = state.masks[surface];
    if (mask.used) return &mask;

    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> converted{
        nullptr, &SDL_DestroySurface};
    SDL_Surface* readable = surface;
    if (surface->format != SDL_PIXELFORMAT_RGBA32) {
        converted.reset(SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32));
        if (!converted) return nullptr;
        readable = converted.get();
    }
    const size_t width = static_cast<size_t>(readable->w);
    const size_t height = static_cast<size_t>(readable->h);
    if (width > mask.pixels.max_size() / height) {
        SDL_SetError("Shadow source exceeds supported dimensions");
        return nullptr;
    }
    mask.pixels.resize(width * height);
    const bool mustLock = SDL_MUSTLOCK(readable);
    if (mustLock && !SDL_LockSurface(readable)) return nullptr;

    const auto* pixels = static_cast<const Uint8*>(readable->pixels);
    for (size_t y = 0; y < height; ++y) {
        const auto* row = pixels + y * static_cast<size_t>(readable->pitch);
        for (size_t x = 0; x < width; ++x)
            mask.pixels[y * width + x] = row[x * 4 + 3];
    }
    if (mustLock) SDL_UnlockSurface(readable);
    mask.width = readable->w;
    mask.height = readable->h;
    mask.used = true;
    return &mask;
}

bool Drawer::CollectOccluders(const ECS::Entity_List& list, float cameraX, float cameraY,
    LightingState& state)
{
    using namespace Core::Managers;
    state.occluders.clear();
    if (state.lights.empty()) return true;

    for (auto [id, entity] : list) {
        if (id == InvalidEntity || entity.Has<ECS::LightTag>() ||
            entity.Has<ECS::CameraComponent>() || !entity.Has<ECS::TransformComponent>() ||
            !entity.Has<ECS::ShadowComponent>()) continue;
        const auto& shadow = entity.Get<ECS::ShadowComponent>();
        if (!shadow.castsShadow) continue;
        if (entity.Has<ECS::VisibilityComponent>() &&
            !entity.Get<ECS::VisibilityComponent>().visible) continue;

        const auto& transform = entity.Get<ECS::TransformComponent>();
        if (!std::isfinite(transform.location.x) || !std::isfinite(transform.location.y) ||
            !std::isfinite(transform.size.x) || !std::isfinite(transform.size.y) ||
            !std::isfinite(transform.rotation) || transform.size.x <= 0.f || transform.size.y <= 0.f)
            continue;

        ShadowOccluder occluder;
        occluder.owner = id;
        occluder.alphaCutoff = std::max<Uint8>(1, shadow.alphaCutoff);
        occluder.rect = {ToScreen(transform.location.x, cameraX),
            ToScreen(transform.location.y, cameraY), transform.size.x, transform.size.y};
        const float radians = std::remainder(transform.rotation, 360.f) * GKC_PI / 180.f;
        occluder.cosine = std::cos(radians);
        occluder.sine = std::sin(radians);
        const float halfWidth = occluder.rect.w * 0.5f;
        const float halfHeight = occluder.rect.h * 0.5f;
        const float centerX = occluder.rect.x + halfWidth;
        const float centerY = occluder.rect.y + halfHeight;
        const float extentX = std::abs(occluder.cosine) * halfWidth + std::abs(occluder.sine) * halfHeight;
        const float extentY = std::abs(occluder.sine) * halfWidth + std::abs(occluder.cosine) * halfHeight;
        occluder.bounds = {centerX - extentX, centerY - extentY, extentX * 2.f, extentY * 2.f};
        if (!std::isfinite(occluder.bounds.x) || !std::isfinite(occluder.bounds.y) ||
            !std::isfinite(occluder.bounds.w) || !std::isfinite(occluder.bounds.h)) continue;

        bool relevant = false;
        for (const auto& light : state.lights) {
            if (light.owner == id) continue;
            const SDL_FRect lightBounds{static_cast<float>(light.x - light.radius),
                static_cast<float>(light.y - light.radius), static_cast<float>(light.radius * 2 + 1),
                static_cast<float>(light.radius * 2 + 1)};
            if (SDL_HasRectIntersectionFloat(&occluder.bounds, &lightBounds)) {
                relevant = true;
                break;
            }
        }
        if (!relevant) continue;

        SDL_Surface* surface = nullptr;
        if (entity.Has<ECS::AnimationComponent>()) {
            auto* animation = AnimationManager::GetAssetByID(entity.Get<ECS::AnimationComponent>().id);
            if (animation != nullptr) {
                if (!animation->IsValid()) continue;
                auto* frames = animation->GetAnimation();
                const int frame = animation->GetCurrentFrame();
                if (frames == nullptr || frame < 0 || frame >= frames->count) continue;
                surface = frames->frames[frame];
                if (surface == nullptr) continue;
            }
            // Missing animation falls back to RenderColor, as in DrawEntities.
        }
        else if (entity.Has<ECS::TextureComponent>()) {
            const auto textureID = entity.Get<ECS::TextureComponent>().id;
            auto* texture = TextureManager::GetAssetByID(textureID);
            const bool registered = TextureManager::GetIDToNameList().find(textureID) !=
                TextureManager::GetIDToNameList().end();
            if (registered && texture != nullptr && texture->GetSDLTexture() != nullptr) {
                surface = texture->GetSDLSurface();
                if (surface == nullptr)
                    return SDL_SetError("Texture shadow source has no CPU surface");
            }
            // The engine's missing-texture checkerboard is opaque.
        }

        if (surface != nullptr) {
            occluder.mask = GetAlphaMask(surface, state);
            if (occluder.mask == nullptr) return false;
        }
        else if (!entity.Has<ECS::TextureComponent>() || entity.Has<ECS::AnimationComponent>()) {
            occluder.alpha = entity.Has<ECS::ColorComponent>()
                ? entity.Get<ECS::ColorComponent>().color.a : GREY_COLOR.a;
            if (occluder.alpha < occluder.alphaCutoff) continue;
        }
        state.occluders.push_back(occluder);
    }
    return true;
}

void Drawer::BuildOcclusion(const SceneLight& light, LightingState& state)
{
    const int side = light.radius * 2 + 1;
    const int left = light.x - light.radius;
    const int top = light.y - light.radius;
    const size_t count = static_cast<size_t>(side) * side;
    state.blocked.assign(count, 0);
    state.visible.assign(count, 0);

    for (const auto& occluder : state.occluders) {
        if (occluder.owner == light.owner) continue;
        const double minX = std::clamp(static_cast<double>(occluder.bounds.x) - left, 0.0,
            static_cast<double>(side));
        const double minY = std::clamp(static_cast<double>(occluder.bounds.y) - top, 0.0,
            static_cast<double>(side));
        const double maxX = std::clamp(static_cast<double>(occluder.bounds.x) + occluder.bounds.w - left,
            0.0, static_cast<double>(side));
        const double maxY = std::clamp(static_cast<double>(occluder.bounds.y) + occluder.bounds.h - top,
            0.0, static_cast<double>(side));
        const int firstX = static_cast<int>(std::floor(minX));
        const int firstY = static_cast<int>(std::floor(minY));
        const int lastX = static_cast<int>(std::ceil(maxX));
        const int lastY = static_cast<int>(std::ceil(maxY));
        const double centerX = static_cast<double>(occluder.rect.x) + occluder.rect.w * 0.5;
        const double centerY = static_cast<double>(occluder.rect.y) + occluder.rect.h * 0.5;

        for (int y = firstY; y < lastY; ++y) {
            const double dy = top + y + 0.5 - centerY;
            for (int x = firstX; x < lastX; ++x) {
                const size_t index = static_cast<size_t>(y) * side + x;
                if (state.blocked[index]) continue;
                const double dx = left + x + 0.5 - centerX;
                const double localX = dx * occluder.cosine + dy * occluder.sine + occluder.rect.w * 0.5;
                const double localY = -dx * occluder.sine + dy * occluder.cosine + occluder.rect.h * 0.5;
                if (localX < 0.0 || localY < 0.0 || localX >= occluder.rect.w || localY >= occluder.rect.h)
                    continue;
                Uint8 alpha = occluder.alpha;
                if (occluder.mask != nullptr) {
                    const auto& mask = *occluder.mask;
                    const int sourceX = std::min(mask.width - 1,
                        static_cast<int>(localX / occluder.rect.w * mask.width));
                    const int sourceY = std::min(mask.height - 1,
                        static_cast<int>(localY / occluder.rect.h * mask.height));
                    alpha = mask.pixels[static_cast<size_t>(sourceY) * mask.width + sourceX];
                }
                if (alpha >= occluder.alphaCutoff) state.blocked[index] = 1;
            }
        }
    }
}

void Drawer::CastLight(LightingState& state, int radius, int row, float startSlope,
    float endSlope, int xx, int xy, int yx, int yy)
{
    if (startSlope < endSlope) return;
    const int side = radius * 2 + 1;
    const int radiusSquared = radius * radius;
    float nextStart = startSlope;
    bool blocked = false;

    for (int distance = row; distance <= radius && !blocked; ++distance) {
        const int dy = -distance;
        for (int dx = -distance; dx <= 0; ++dx) {
            const float leftSlope = (dx - 0.5f) / (dy + 0.5f);
            const float rightSlope = (dx + 0.5f) / (dy - 0.5f);
            if (startSlope < rightSlope) continue;
            if (endSlope > leftSlope) break;
            const int x = radius + dx * xx + dy * xy;
            const int y = radius + dx * yx + dy * yy;
            const size_t index = static_cast<size_t>(y) * side + x;
            if (dx * dx + dy * dy <= radiusSquared) state.visible[index] = 1;
            const bool opaque = state.blocked[index] != 0;
            if (blocked) {
                if (opaque) nextStart = rightSlope;
                else {
                    blocked = false;
                    startSlope = nextStart;
                }
            }
            else if (opaque && distance < radius) {
                blocked = true;
                CastLight(state, radius, distance + 1, startSlope, leftSlope, xx, xy, yx, yy);
                nextStart = rightSlope;
            }
        }
    }
}

float Drawer::CalculateLightIntensity(const SceneLight& light, float distanceSquared)
{
    if (distanceSquared >= light.radiusSquared) return 0.f;
    const float falloff = 1.f - distanceSquared / light.radiusSquared;
    return light.intensity * falloff * falloff;
}

void Drawer::AccumulateLight(const SceneLight& light, LightingState& state)
{
    const int side = light.radius * 2 + 1;
    const int left = light.x - light.radius;
    const int top = light.y - light.radius;
    const int firstX = std::max(0, left);
    const int firstY = std::max(0, top);
    const int lastX = std::min(state.width, light.x + light.radius + 1);
    const int lastY = std::min(state.height, light.y + light.radius + 1);
    for (int y = firstY; y < lastY; ++y) {
        const float dy = static_cast<float>(y - light.y);
        for (int x = firstX; x < lastX; ++x) {
            const size_t visibilityIndex = static_cast<size_t>(y - top) * side + (x - left);
            if (!state.visible[visibilityIndex]) continue;
            const float dx = static_cast<float>(x - light.x);
            const float intensity = CalculateLightIntensity(light, dx * dx + dy * dy);
            if (intensity <= 0.f) continue;
            const size_t index = (static_cast<size_t>(y) * state.width + x) * 3;
            state.brightness[index] += light.color.r * intensity;
            state.brightness[index + 1] += light.color.g * intensity;
            state.brightness[index + 2] += light.color.b * intensity;
        }
    }
}

bool Drawer::DrawLighting(const ECS::Entity_List& list, SDL_Renderer* renderer,
    Core::Systems::CameraSystem& cameraSystem, LightingState& state,
    int viewWidth, int viewHeight, float ambientLight)
{
    if (!std::isfinite(ambientLight)) return SDL_SetError("Ambient light must be finite");
    ambientLight = std::clamp(ambientLight, 0.f, 1.f);
    if (ambientLight >= 1.f) return true;
    if (!PrepareLighting(state, renderer, viewWidth, viewHeight)) return false;
    auto cameraEntity = cameraSystem.GetActiveCamera();
    if (!cameraEntity.Has<ECS::CameraComponent>())
        return SDL_SetError("Active camera has no CameraComponent");
    const auto& camera = cameraEntity.Get<ECS::CameraComponent>();
    if (!std::isfinite(camera.location.x) || !std::isfinite(camera.location.y))
        return SDL_SetError("Camera location must be finite");

    state.occluders.clear();
    for (auto& entry : state.masks) entry.second.used = false;
    if (!CollectLights(list, camera.location.x, camera.location.y, state) ||
        !CollectOccluders(list, camera.location.x, camera.location.y, state)) return false;
    std::erase_if(state.masks, [](const auto& entry) { return !entry.second.used; });
    std::fill(state.brightness.begin(), state.brightness.end(), ambientLight);

    static constexpr int sectors[8][4] = {
        {1, 0, 0, 1}, {0, 1, 1, 0}, {0, -1, 1, 0}, {-1, 0, 0, 1},
        {-1, 0, 0, -1}, {0, -1, -1, 0}, {0, 1, -1, 0}, {1, 0, 0, -1}
    };
    for (const auto& light : state.lights) {
        BuildOcclusion(light, state);
        const int side = light.radius * 2 + 1;
        const size_t center = static_cast<size_t>(light.radius) * side + light.radius;
        if (state.blocked[center]) continue;
        state.visible[center] = 1;
        for (const auto& sector : sectors)
            CastLight(state, light.radius, 1, 1.f, 0.f, sector[0], sector[1], sector[2], sector[3]);
        AccumulateLight(light, state);
    }

    const size_t pixelCount = static_cast<size_t>(state.width) * state.height;
    for (size_t pixel = 0; pixel < pixelCount; ++pixel) {
        for (size_t channel = 0; channel < 3; ++channel) {
            const float brightness = std::clamp(state.brightness[pixel * 3 + channel], 0.f, 1.f);
            state.output[pixel * 4 + channel] = static_cast<Uint8>(brightness * 255.f + 0.5f);
        }
        state.output[pixel * 4 + 3] = 255;
    }
    if (!SDL_UpdateTexture(state.texture.get(), nullptr, state.output.data(), state.width * 4))
        return false;
    const SDL_FRect destination{0.f, 0.f, static_cast<float>(state.width), static_cast<float>(state.height)};
    return SDL_RenderTexture(renderer, state.texture.get(), nullptr, &destination);
}
