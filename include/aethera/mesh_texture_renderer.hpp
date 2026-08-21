#pragma once

#include <SDL.h>
#include <cstdint>
#include <vector>

#include "aethera/image.hpp"
#include "aethera/mesh_renderer.hpp"

namespace aethera {

class MeshTextureRenderer {
public:
    explicit MeshTextureRenderer(SDL_Renderer* renderer) : renderer_(renderer) {}

    bool draw(const ImageRgba8& image, const MeshDrawData& mesh, Vec2 offset = {});

private:
    SDL_Renderer* renderer_{};
};

} // namespace aethera
