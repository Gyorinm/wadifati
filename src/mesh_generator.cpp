#include "aethera/mesh_generator.hpp"

#include <algorithm>

namespace aethera {

std::vector<MeshVertex> MeshGenerator::grid_vertices(const ImageRgba8& layer, int columns, int rows) {
    columns = std::max(1, columns);
    rows = std::max(1, rows);
    std::vector<MeshVertex> vertices;
    vertices.reserve(static_cast<std::size_t>((columns + 1) * (rows + 1)));

    for (int y = 0; y <= rows; ++y) {
        for (int x = 0; x <= columns; ++x) {
            const float u = static_cast<float>(x) / static_cast<float>(columns);
            const float v = static_cast<float>(y) / static_cast<float>(rows);
            MeshVertex vertex;
            vertex.position = {u * static_cast<float>(layer.width), v * static_cast<float>(layer.height)};
            vertex.uv = {u, v};
            // Initial binding: the caller can replace these with skeleton-derived weights.
            vertex.bone0 = 0;
            vertex.bone1 = 0;
            vertex.weight0 = 1.0f;
            vertex.weight1 = 0.0f;
            vertices.push_back(vertex);
        }
    }
    return vertices;
}

std::vector<std::uint32_t> MeshGenerator::grid_indices(int columns, int rows) {
    columns = std::max(1, columns);
    rows = std::max(1, rows);
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
