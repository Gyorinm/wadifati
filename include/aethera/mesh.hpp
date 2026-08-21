#pragma once

#include <cstdint>
#include <vector>

#include "aethera/math.hpp"

namespace aethera {

struct MeshVertex {
    Vec2 position{};
    Vec2 uv{};
    std::uint32_t bone0{0};
    std::uint32_t bone1{0};
    float weight0{1.0f};
    float weight1{0.0f};
};

struct MeshBone {
    std::uint32_t index{0};
    Vec2 bind_position{};
    Vec2 current_position{};
    float bind_rotation{0.0f};
    float current_rotation{0.0f};
};

struct DeformedVertex {
    Vec2 position{};
    Vec2 uv{};
};

class MeshDeformer {
public:
    static Vec2 transform_vertex(const MeshVertex& vertex, const std::vector<MeshBone>& bones);
    static std::vector<DeformedVertex> deform(const std::vector<MeshVertex>& vertices,
                                               const std::vector<MeshBone>& bones);
};

} // namespace aethera
