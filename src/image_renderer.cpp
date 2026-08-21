#include "aethera/image_renderer.hpp"

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
    for (const auto& entry : textures_) {
        if (entry.second != nullptr) {
            SDL_DestroyTexture(entry.second);
        }
    }
    textures_.clear();
}

SDL_Texture* ImageRenderer::texture_for(const ImageRgba8& image) {
    if (!image.valid() || renderer_ == nullptr) {
        return nullptr;
    }

    const auto found = textures_.find(&image);
    if (found != textures_.end()) {
        return found->second;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STATIC,
                                              image.width, image.height);
    if (texture == nullptr) {
        return nullptr;
    }

    if (SDL_UpdateTexture(texture, nullptr, image.data(), image.width * 4) != 0) {
        SDL_DestroyTexture(texture);
        return nullptr;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    textures_.emplace(&image, texture);
    return texture;
}

bool ImageRenderer::draw(const ImageObject& object) {
    if (renderer_ == nullptr) {
        return false;
    }

    for (std::size_t i = 0; i < object.nodes().size(); ++i) {
        const ImageNode& node = object.nodes()[i];
        if (!node.visible) {
            continue;
        }

        const ImageRgba8* image = object.image_for_node(i);
        if (image == nullptr || !image->valid()) {
            continue;
        }

        SDL_Texture* texture = texture_for(*image);
        if (texture == nullptr) {
            return false;
        }

        const ImageTransform& transform = object.world_transform(i);
        const Rect& source = node.visual.source;
        const bool use_source = source.width > 0.0f && source.height > 0.0f;
        SDL_Rect src = use_source ? to_rect(source) : SDL_Rect{0, 0, image->width, image->height};

        const float source_width = use_source ? source.width : static_cast<float>(image->width);
        const float source_height = use_source ? source.height : static_cast<float>(image->height);
        const Vec2& pivot = node.visual.pivot;

        SDL_FRect dst;
        dst.w = source_width * transform.scale.x;
        dst.h = source_height * transform.scale.y;
        dst.x = transform.position.x - pivot.x * dst.w;
        dst.y = transform.position.y - pivot.y * dst.h;

        SDL_FPoint center{
            pivot.x * dst.w,
            pivot.y * dst.h
        };

        const float opacity = clamp(node.visual.opacity, 0.0f, 1.0f);
        SDL_SetTextureAlphaMod(texture, static_cast<Uint8>(std::lround(opacity * 255.0f)));

        if (SDL_RenderCopyExF(renderer_, texture, &src, &dst,
                               transform.rotation * 57.2957795f,
                               &center, SDL_FLIP_NONE) != 0) {
            SDL_SetTextureAlphaMod(texture, 255);
            return false;
        }
    }

    for (auto& entry : textures_) {
        SDL_SetTextureAlphaMod(entry.second, 255);
    }
    return true;
}

} // namespace aethera
