#pragma once

#include <vector>

#include "aethera/image.hpp"
#include "aethera/mesh.hpp"

namespace aethera {

class MeshGenerator {
public:
    // Builds a regular triangulated grid over the visible bounds of an RGBA layer.
    // Alpha is used to discard vertices outside the visible region.
    static std::vector<MeshVertex> grid_vertices(const ImageRgba8& layer, int columns = 8, int rows = 12);
    static std::vector<std::uint32_t> grid_indices(int columns, int rows);
};

} // namespace aethera
