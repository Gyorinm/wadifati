#include "aethera/image_renderer.hpp"

#include <algorithm>
#include <cmath>

namespace aethera {

namespace {

SDL_Rect to_rect(const Rect& r) {
    return {
        static_cast<int>(std::lround(r.x)),
        static_cast<int>(std::lround(r.y)),
        static_cast<int>(std::lround(r.width)),
        static_cast<int>(std::lround(r.height))
    };
}

} // namespace

ImageRenderer::~ImageRenderer() {
    clear_cache();
}

void ImageRenderer::clear_cache() {
    if (cached_texture_ != nullptr) {
        SDL_DestroyTexture(cached_texture_);
        cached_texture_ = nullptr;
    }
    cached_image_ = nullptr;
}

SDL_Texture* ImageRenderer::texture_for(const ImageRgba8& image) {
    if (!image.valid() || renderer_ == nullptr) {
        return nullptr;
    }

    if (cached_image_ == &image && cached_texture_ != nullptr) {
        return cached_texture_;
    }

    clear_cache();

    cached_texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
                                        SDL_TEXTUREACCESS_STATIC, image.width, image.height);
    if (cached_texture_ == nullptr) {
        return nullptr;
    }

    if (SDL_UpdateTexture(cached_texture_, nullptr, image.data(), image.width * 4) != 0) {
        clear_cache();
        return nullptr;
    }

    SDL_SetTextureBlendMode(cached_texture_, SDL_BLENDMODE_BLEND);
    cached_image_ = &image;
    return cached_texture_;
}

bool ImageRenderer::draw(const ImageObject& object) {
    if (renderer_ == nullptr || object.image() == nullptr || !object.image()->valid()) {
        return false;
    }

    SDL_Texture* texture = texture_for(*object.image());
    if (texture == nullptr) {
        return false;
    }

    for (std::size_t i = 0; i < object.nodes().size(); ++i) {
        const ImageNode& node = object.nodes()[i];
        if (!node.visible || node.visual.source.width <= 0.0f || node.visual.source.height <= 0.0f) {
            continue;
        }

        const ImageTransform& transform = object.world_transform(i);
        const Rect& source = node.visual.source;
        const Vec2& pivot = node.visual.pivot;

        SDL_Rect src = to_rect(source);
        SDL_FRect dst;
        dst.w = source.width * transform.scale.x;
        dst.h = source.height * transform.scale.y;
        dst.x = transform.position.x - pivot.x * dst.w;
        dst.y = transform.position.y - pivot.y * dst.h;

        SDL_FPoint center{
            pivot.x * dst.w,
            pivot.y * dst.h
        };

        const float opacity = clamp(node.visual.opacity, 0.0f, 1.0f);
        SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(std::lround(opacity * 255.0f)));

        if (SDL_RenderCopyExF(renderer_, texture, &src, &dst,
                               transform.rotation * 57.2957795,
                               &center, SDL_FLIP_NONE) != 0) {
            return false;
        }
    }

    SDL_SetTextureAlphaMod(texture, 255);
    return true;
}

} // namespace aethera
