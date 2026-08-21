#pragma once
#include <SDL.h>
#include "aethera/image_pipeline.hpp"
namespace aethera {
class ImageActorRenderer {
public:
    explicit ImageActorRenderer(SDL_Renderer* renderer) : renderer_(renderer) {}
    bool draw(const ImageActor& actor, float x, float y);
private:
    SDL_Renderer* renderer_{};
};
}
