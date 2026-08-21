#pragma once

#include <vector>
#include "aethera/mesh.hpp"
#include "aethera/vision_model.hpp"

namespace aethera {

// Converts normalized keypoints into layer-space coordinates and prepares bone bind positions.
std::vector<MeshBone> build_bones_from_keypoints(const std::vector<VisionKeypoint>& keypoints,
                                                  int layer_width, int layer_height);

void assign_layer_bone_weights(std::vector<MeshVertex>& vertices,
                               std::vector<MeshBone>& bones,
                               const std::vector<VisionKeypoint>& keypoints,
                               int layer_width, int layer_height);

} // namespace aethera
