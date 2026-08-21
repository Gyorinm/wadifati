#pragma once

#include <vector>

#include "aethera/mesh.hpp"
#include "aethera/vision_model.hpp"

namespace aethera {

// Assigns up to two nearest keypoint/bone influences to every mesh vertex.
void assign_nearest_bone_weights(std::vector<MeshVertex>& vertices,
                                 const std::vector<MeshBone>& bones,
                                 const std::vector<VisionKeypoint>& keypoints);

} // namespace aethera
