#include <cassert>
#include "aethera/adaptive_mesh.hpp"
using namespace aethera;
int main() {
    ImageRgba8 image; image.width = 160; image.height = 120; image.pixels.resize(160u * 120u * 4u, 255);
    AdaptiveMeshConfig config; config.pixels_per_cell = 20; config.min_columns = 4; config.max_columns = 16; config.min_rows = 4; config.max_rows = 16;
    const auto vertices = AdaptiveMeshGenerator::generate_vertices(image, config);
    assert(vertices.size() == static_cast<std::size_t>((8 + 1) * (6 + 1)));
    const auto indices = AdaptiveMeshGenerator::generate_indices(8, 6);
    assert(indices.size() == static_cast<std::size_t>(8 * 6 * 6));
    assert(vertices.front().uv.x == 0.0f && vertices.front().uv.y == 0.0f);
    assert(vertices.back().uv.x == 1.0f && vertices.back().uv.y == 1.0f);
    return 0;
}
