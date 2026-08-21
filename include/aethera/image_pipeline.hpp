#pragma once
#include <vector>
#include "aethera/image.hpp"
#include "aethera/mesh.hpp"
#include "aethera/skeleton_pose.hpp"
namespace aethera {
struct ImageActor {
    ImageRgba8 image;
    std::vector<MeshVertex> mesh;
    std::vector<std::uint32_t> indices;
    std::vector<SkeletonJoint> skeleton;
    std::vector<MeshBone> bones;
};
class ImagePipeline {
public:
    static ImageActor build(const ImageRgba8& image, int columns=12, int rows=16);
    static void update_bones(ImageActor& actor);
};
}
