#pragma once

#include <cstdint>
#include <vector>
#include "aethera/image.hpp"
#include "aethera/mesh.hpp"

namespace aethera {

struct AdaptiveMeshConfig {
    int min_columns{4};
    int max_columns{32};
    int min_rows{4};
    int max_rows{32};
    int pixels_per_cell{24};
};

class AdaptiveMeshGenerator {
public:
    static std::vector<MeshVertex> generate_vertices(const ImageRgba8& image,
                                                      const AdaptiveMeshConfig& config = {});
    static std::vector<std::uint32_t> generate_indices(int columns, int rows);
};

} // namespace aethera
