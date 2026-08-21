#include "aethera/texture_loader.hpp"

#ifdef AETHERA_HAS_SDL_IMAGE
#include <SDL_image.h>
#endif

namespace aethera {

SDL_Texture* TextureLoader::load(SDL_Renderer* renderer, const std::string& path) {
    if (!renderer || path.empty()) return nullptr;
#ifdef AETHERA_HAS_SDL_IMAGE
    SDL_Surface* surface = IMG_Load(path.c_str());
#else
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
#endif
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

} // namespace aethera
