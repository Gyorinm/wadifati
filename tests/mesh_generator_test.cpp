#include <cassert>
#include <cstdint>

#include "aethera/mesh_generator.hpp"

using namespace aethera;

int main() {
    ImageRgba8 image;
    image.width = 32; image.height = 24;
    image.pixels.resize(static_cast<std::size_t>(32 * 24 * 4), 255);

    const auto vertices = MeshGenerator::grid_vertices(image, 4, 3);
    const auto indices = MeshGenerator::grid_indices(4, 3);
    assert(vertices.size() == 20);
    assert(indices.size() == 72);
    assert(vertices.front().position.x == 0.0f);
    assert(vertices.back().position.x == 32.0f);
    assert(vertices.back().position.y == 24.0f);
    assert(vertices.back().uv.x == 1.0f);
    assert(vertices.back().uv.y == 1.0f);
    return 0;
}
