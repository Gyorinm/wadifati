#include "aethera/mesh_texture_renderer.hpp"

#include <algorithm>

namespace aethera {

bool MeshTextureRenderer::draw(const ImageRgba8& image, const MeshDrawData& mesh) {
    if (renderer_ == nullptr || !image.valid() || mesh.vertices.empty() || mesh.indices.empty()) return false;
    SDL_Texture* texture = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STATIC, image.width, image.height);
    if (texture == nullptr) return false;
    if (SDL_UpdateTexture(texture, nullptr, image.data(), image.width * 4) != 0) {
        SDL_DestroyTexture(texture);
        return false;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    std::vector<SDL_Vertex> vertices;
    vertices.reserve(mesh.vertices.size());
    for (const MeshDrawVertex& v : mesh.vertices) {
        SDL_Vertex out{};
        out.position = {v.x, v.y};
        out.color = {255, 255, 255, 255};
        out.tex_coord = {v.u, v.v};
        vertices.push_back(out);
    }

    const int vertex_count = static_cast<int>(std::min<std::size_t>(vertices.size(), 65535));
    std::vector<int> indices;
    indices.reserve(mesh.indices.size());
    for (std::uint32_t index : mesh.indices) {
        if (index >= static_cast<std::uint32_t>(vertex_count)) continue;
        indices.push_back(static_cast<int>(index));
    }
    const bool ok = !indices.empty() &&
        SDL_RenderGeometry(renderer_, texture, vertices.data(), vertex_count,
                           indices.data(), static_cast<int>(indices.size())) == 0;
    SDL_DestroyTexture(texture);
    return ok;
}

} // namespace aethera
