#include "aethera/mesh_renderer.hpp"

#include <algorithm>

namespace aethera {

MeshDrawData MeshRenderer::build_draw_data(const std::vector<MeshVertex>& vertices,
                                           const std::vector<DeformedVertex>& deformed,
                                           const std::vector<std::uint32_t>& indices) const {
    MeshDrawData data;
    const std::size_t count = std::min(vertices.size(), deformed.size());
    data.vertices.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        data.vertices.push_back({deformed[i].position.x, deformed[i].position.y,
                                 vertices[i].uv.x, vertices[i].uv.y});
    }
    data.indices = indices;
    return data;
}

} // namespace aethera
