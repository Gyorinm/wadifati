#include "aethera/image_loader.hpp"
#include <SDL.h>
#ifdef AETHERA_HAS_SDL_IMAGE
#include <SDL_image.h>
#endif
namespace aethera {
bool ImageLoader::load(const std::string& path, ImageRgba8& out) {
    out = {};
    SDL_Surface* src = nullptr;
#ifdef AETHERA_HAS_SDL_IMAGE
    src = IMG_Load(path.c_str());
#else
    src = SDL_LoadBMP(path.c_str());
#endif
    if (!src) return false;
    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(src, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(src);
    if (!rgba) return false;
    out.width = rgba->w; out.height = rgba->h;
    out.pixels.resize(static_cast<std::size_t>(out.width) * static_cast<std::size_t>(out.height) * 4);
    for (int y = 0; y < out.height; ++y) {
        const auto* row = static_cast<const std::uint8_t*>(rgba->pixels) + static_cast<std::size_t>(y) * rgba->pitch;
        auto* dst = out.pixels.data() + static_cast<std::size_t>(y) * out.width * 4;
        std::memcpy(dst, row, static_cast<std::size_t>(out.width) * 4);
    }
    SDL_FreeSurface(rgba);
    return out.valid();
}
}
