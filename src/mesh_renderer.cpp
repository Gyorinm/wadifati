#include "aethera/mesh_renderer.hpp"
#include <SDL.h>
#include <cmath>

namespace aethera {

MeshDrawData MeshRenderer::build(const std::vector<MeshVertex>& source,
                                 const std::vector<MeshBone>& bones,
                                 const std::vector<std::uint32_t>& indices,
                                 Vec2 offset) {
    const auto deformed = MeshDeformer::deform(source, bones);
    MeshDrawData data;
    data.indices = indices;
    data.vertices.reserve(deformed.size());
    for (const auto& v : deformed) {
        data.vertices.push_back({v.position.x + offset.x, v.position.y + offset.y, v.uv.x, v.uv.y});
    }
    return data;
}

bool MeshRenderer::draw(SDL_Renderer* renderer, SDL_Texture* texture, const MeshDrawData& data) {
    if (!renderer || !texture || data.vertices.empty() || data.indices.empty()) return false;
    std::vector<SDL_Vertex> vertices;
    vertices.reserve(data.vertices.size());
    for (const auto& v : data.vertices) {
        SDL_Vertex out{};
        out.position = {v.x, v.y};
        out.tex_coord = {v.u, v.v};
        out.color = {255, 255, 255, 255};
        vertices.push_back(out);
    }
    return SDL_RenderGeometry(renderer, texture, vertices.data(), static_cast<int>(vertices.size()),
                              reinterpret_cast<const int*>(data.indices.data()), static_cast<int>(data.indices.size())) == 0;
}

} // namespace aethera
