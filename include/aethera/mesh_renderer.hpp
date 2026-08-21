#pragma once

#include <SDL.h>
#include <cstdint>
#include <vector>
#include "aethera/mesh.hpp"

namespace aethera {

struct MeshDrawVertex {
    float x{0.0f};
    float y{0.0f};
    float u{0.0f};
    float v{0.0f};
};

struct MeshDrawData {
    std::vector<MeshDrawVertex> vertices;
    std::vector<std::uint32_t> indices;
};

class MeshRenderer {
public:
    static MeshDrawData build(const std::vector<MeshVertex>& source,
                              const std::vector<MeshBone>& bones,
                              const std::vector<std::uint32_t>& indices,
                              Vec2 offset = {});
    static bool draw(SDL_Renderer* renderer, SDL_Texture* texture, const MeshDrawData& data);
};

} // namespace aethera
