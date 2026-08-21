#include "aethera/image_actor_renderer.hpp"
#include "aethera/image_loader.hpp"
#include "aethera/mesh_renderer.hpp"
#include <SDL.h>
namespace aethera {
bool ImageActorRenderer::draw(const ImageActor& actor, float x, float y) {
    if (!renderer_ || !actor.image.valid() || actor.mesh.empty() || actor.indices.empty()) return false;
    SDL_Texture* texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC,
                                             actor.image.width, actor.image.height);
    if (!texture) return false;
    if (SDL_UpdateTexture(texture, nullptr, actor.image.pixels.data(), actor.image.width * 4) != 0) {
        SDL_DestroyTexture(texture); return false;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    const auto data = MeshRenderer::build(actor.mesh, actor.bones, actor.indices, {x, y});
    const bool ok = MeshRenderer::draw(renderer_, texture, data);
    SDL_DestroyTexture(texture);
    return ok;
}
}
