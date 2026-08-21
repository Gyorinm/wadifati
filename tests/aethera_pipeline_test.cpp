#include <cassert>
#include <cmath>
#include <vector>
#include "aethera/image.hpp"
#include "aethera/shape_mesh.hpp"
#include "aethera/skeleton_pose.hpp"
int main(){
 aethera::ImageRgba8 img; img.width=32; img.height=32; img.pixels.resize(32*32*4,255);
 auto v=aethera::ShapeMeshGenerator::vertices_from_alpha(img,4,4); auto i=aethera::ShapeMeshGenerator::indices(4,4);
 assert(v.size()==25); assert(i.size()==96);
 std::vector<aethera::SkeletonJoint> j(2); j[0].bind={0,0};j[0].current={0,0};j[1].parent=0;j[1].bind={16,0};j[1].current={16,0};j[0].current_rotation=0.25f;
 aethera::SkeletonPose::propagate(j); auto bones=aethera::SkeletonPose::to_mesh_bones(j); assert(bones.size()==2); assert(std::isfinite(bones[1].current_position.x)); return 0;
}
