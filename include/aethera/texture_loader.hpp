#pragma once

#include <SDL.h>
#include <string>

namespace aethera {

class TextureLoader {
public:
    // Loads common image formats when SDL_image is available; BMP is always supported by SDL2.
    static SDL_Texture* load(SDL_Renderer* renderer, const std::string& path);
};

} // namespace aethera
