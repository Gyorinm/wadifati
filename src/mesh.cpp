#include "aethera/mesh.hpp"

#include <algorithm>
#include <cmath>

namespace aethera {
namespace {

Vec2 rotate_offset(const Vec2& p, float angle) {
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    return {p.x * c - p.y * s, p.x * s + p.y * c};
}

Vec2 bone_transform(const Vec2& p, const MeshBone& bone) {
    const Vec2 relative = p - bone.bind_position;
    const Vec2 rotated = rotate_offset(relative, bone.current_rotation - bone.bind_rotation);
    return bone.current_position + rotated;
}

} // namespace

Vec2 MeshDeformer::transform_vertex(const MeshVertex& vertex, const std::vector<MeshBone>& bones) {
    if (bones.empty()) return vertex.position;
    const std::size_t i0 = std::min<std::size_t>(vertex.bone0, bones.size() - 1);
    const std::size_t i1 = std::min<std::size_t>(vertex.bone1, bones.size() - 1);
    const Vec2 p0 = bone_transform(vertex.position, bones[i0]);
    const Vec2 p1 = bone_transform(vertex.position, bones[i1]);
    const float w0 = std::clamp(vertex.weight0, 0.0f, 1.0f);
    const float w1 = std::clamp(vertex.weight1, 0.0f, 1.0f);
    const float total = w0 + w1;
    if (total <= 0.0f) return vertex.position;
    return (p0 * w0 + p1 * w1) / total;
}

std::vector<DeformedVertex> MeshDeformer::deform(const std::vector<MeshVertex>& vertices,
                                                  const std::vector<MeshBone>& bones) {
    std::vector<DeformedVertex> result;
    result.reserve(vertices.size());
    for (const MeshVertex& vertex : vertices) {
        result.push_back({transform_vertex(vertex, bones), vertex.uv});
    }
    return result;
}

} // namespace aethera
