#include "aethera/mesh_weights.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace aethera {

void assign_nearest_bone_weights(std::vector<MeshVertex>& vertices,
                                 const std::vector<MeshBone>& bones,
                                 const std::vector<VisionKeypoint>& keypoints) {
    if (bones.empty()) return;
    for (MeshVertex& vertex : vertices) {
        struct Candidate { std::size_t index; float distance; } best[2] = {
            {0, std::numeric_limits<float>::max()}, {0, std::numeric_limits<float>::max()}};
        for (std::size_t i = 0; i < bones.size(); ++i) {
            Vec2 p = bones[i].bind_position;
            if (i < keypoints.size() && keypoints[i].confidence > 0.0f) {
                p = keypoints[i].normalized_position;
            }
            const float d = length(vertex.position - p);
            if (d < best[0].distance) { best[1] = best[0]; best[0] = {i, d}; }
            else if (d < best[1].distance) best[1] = {i, d};
        }
        vertex.bone0 = static_cast<std::uint32_t>(best[0].index);
        vertex.bone1 = static_cast<std::uint32_t>(best[1].index);
        const float inv0 = 1.0f / std::max(best[0].distance, 0.001f);
        const float inv1 = 1.0f / std::max(best[1].distance, 0.001f);
        const float sum = inv0 + inv1;
        vertex.weight0 = inv0 / sum;
        vertex.weight1 = inv1 / sum;
    }
}

} // namespace aethera
