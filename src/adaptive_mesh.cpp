#include "aethera/adaptive_mesh.hpp"
#include <algorithm>
#include <cmath>

namespace aethera {

std::vector<MeshVertex> AdaptiveMeshGenerator::generate_vertices(const ImageRgba8& image,
                                                                   const AdaptiveMeshConfig& config) {
    const int safe_ppc = std::max(1, config.pixels_per_cell);
    int columns = std::max(config.min_columns, (image.width + safe_ppc - 1) / safe_ppc);
    int rows = std::max(config.min_rows, (image.height + safe_ppc - 1) / safe_ppc);
    columns = std::min(columns, std::max(config.min_columns, config.max_columns));
    rows = std::min(rows, std::max(config.min_rows, config.max_rows));

    std::vector<MeshVertex> vertices;
    vertices.reserve(static_cast<std::size_t>((columns + 1) * (rows + 1)));
    for (int y = 0; y <= rows; ++y) {
        const float v = static_cast<float>(y) / static_cast<float>(rows);
        for (int x = 0; x <= columns; ++x) {
            const float u = static_cast<float>(x) / static_cast<float>(columns);
            MeshVertex vertex;
            vertex.position = {u * static_cast<float>(image.width), v * static_cast<float>(image.height)};
            vertex.uv = {u, v};
            vertices.push_back(vertex);
        }
    }
    return vertices;
}

std::vector<std::uint32_t> AdaptiveMeshGenerator::generate_indices(int columns, int rows) {
    columns = std::max(1, columns); rows = std::max(1, rows);
    std::vector<std::uint32_t> indices;
    indices.reserve(static_cast<std::size_t>(columns * rows * 6));
    const int stride = columns + 1;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            const std::uint32_t a = static_cast<std::uint32_t>(y * stride + x);
            const std::uint32_t b = a + 1;
            const std::uint32_t c = a + static_cast<std::uint32_t>(stride);
            const std::uint32_t d = c + 1;
            indices.insert(indices.end(), {a, c, b, b, c, d});
        }
    }
    return indices;
}

} // namespace aethera
