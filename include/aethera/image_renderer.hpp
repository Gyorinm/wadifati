#pragma once

#include <SDL.h>

#include "aethera/image.hpp"
#include "aethera/image_object.hpp"

namespace aethera {

class ImageRenderer {
public:
    explicit ImageRenderer(SDL_Renderer* renderer) : renderer_(renderer) {}
    ~ImageRenderer();

    ImageRenderer(const ImageRenderer&) = delete;
    ImageRenderer& operator=(const ImageRenderer&) = delete;

    bool draw(const ImageObject& object);
    void clear_cache();

private:
    SDL_Texture* texture_for(const ImageRgba8& image);

    SDL_Renderer* renderer_{};
    const ImageRgba8* cached_image_{};
    SDL_Texture* cached_texture_{};
};

} // namespace aethera
