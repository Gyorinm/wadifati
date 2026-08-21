#pragma once
#include <vector>
#include "aethera/mesh.hpp"
namespace aethera {
struct SkeletonJoint { int parent{-1}; Vec2 bind{}; Vec2 current{}; float bind_rotation{0}; float current_rotation{0}; };
class SkeletonPose {
public:
 static void propagate(std::vector<SkeletonJoint>& joints);
 static std::vector<MeshBone> to_mesh_bones(const std::vector<SkeletonJoint>& joints);
};
}
