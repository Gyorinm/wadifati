#pragma once

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
    MeshDrawData build_draw_data(const std::vector<MeshVertex>& vertices,
                                 const std::vector<DeformedVertex>& deformed,
                                 const std::vector<std::uint32_t>& indices) const;
};

} // namespace aethera
