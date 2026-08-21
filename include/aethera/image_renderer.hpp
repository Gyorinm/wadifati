#pragma once

#include <SDL.h>

#include <unordered_map>

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
    std::unordered_map<const ImageRgba8*, SDL_Texture*> textures_;
};

} // namespace aethera
