#include "aethera/bone_binding.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace aethera {

std::vector<MeshBone> build_bones_from_keypoints(const std::vector<VisionKeypoint>& keypoints,
                                                  int layer_width, int layer_height) {
    std::vector<MeshBone> bones;
    bones.reserve(keypoints.size());
    for (std::size_t i = 0; i < keypoints.size(); ++i) {
        MeshBone bone;
        bone.index = static_cast<std::uint32_t>(i);
        bone.bind_position = {keypoints[i].normalized_position.x * layer_width,
                              keypoints[i].normalized_position.y * layer_height};
        bone.current_position = bone.bind_position;
        bones.push_back(bone);
    }
    return bones;
}

void assign_layer_bone_weights(std::vector<MeshVertex>& vertices,
                               std::vector<MeshBone>& bones,
                               const std::vector<VisionKeypoint>& keypoints,
                               int layer_width, int layer_height) {
    if (bones.empty()) return;
    for (MeshBone& bone : bones) {
        if (bone.index < keypoints.size()) {
            bone.bind_position = {keypoints[bone.index].normalized_position.x * layer_width,
                                  keypoints[bone.index].normalized_position.y * layer_height};
            bone.current_position = bone.bind_position;
        }
    }
    for (MeshVertex& vertex : vertices) {
        std::size_t first = 0, second = 0;
        float d0 = std::numeric_limits<float>::max(), d1 = d0;
        for (std::size_t i = 0; i < bones.size(); ++i) {
            const float dx = vertex.position.x - bones[i].bind_position.x;
            const float dy = vertex.position.y - bones[i].bind_position.y;
            const float d = std::sqrt(dx * dx + dy * dy);
            if (d < d0) { second = first; d1 = d0; first = i; d0 = d; }
            else if (d < d1) { second = i; d1 = d; }
        }
        const float a = 1.0f / std::max(d0, 0.001f);
        const float b = 1.0f / std::max(d1, 0.001f);
        vertex.bone0 = static_cast<std::uint32_t>(first);
        vertex.bone1 = static_cast<std::uint32_t>(second);
        vertex.weight0 = a / (a + b);
        vertex.weight1 = b / (a + b);
    }
}

} // namespace aethera
