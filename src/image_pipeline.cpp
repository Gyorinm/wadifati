#include "aethera/image_pipeline.hpp"
#include "aethera/shape_mesh.hpp"
#include <algorithm>
#include <cmath>
namespace aethera {
ImageActor ImagePipeline::build(const ImageRgba8& image, int columns, int rows) {
    ImageActor actor; actor.image = image;
    actor.mesh = ShapeMeshGenerator::vertices_from_alpha(actor.image, columns, rows, 8);
    actor.indices = ShapeMeshGenerator::indices(columns, rows);
    const float w = static_cast<float>(std::max(1, image.width));
    const float h = static_cast<float>(std::max(1, image.height));
    actor.skeleton.resize(2);
    actor.skeleton[0].parent = -1;
    actor.skeleton[0].bind = {w * 0.5f, h * 0.5f};
    actor.skeleton[0].current = actor.skeleton[0].bind;
    actor.skeleton[1].parent = 0;
    actor.skeleton[1].bind = {w * 0.75f, h * 0.5f};
    actor.skeleton[1].current = actor.skeleton[1].bind;
    actor.bones = SkeletonPose::to_mesh_bones(actor.skeleton);
    for (auto& v : actor.mesh) {
        const float d0 = length(v.position - actor.skeleton[0].bind);
        const float d1 = length(v.position - actor.skeleton[1].bind);
        const float a = 1.0f / std::max(d0, 0.001f), b = 1.0f / std::max(d1, 0.001f);
        v.bone0 = 0; v.bone1 = 1; v.weight0 = a / (a+b); v.weight1 = b / (a+b);
    }
    return actor;
}
void ImagePipeline::update_bones(ImageActor& actor) {
    SkeletonPose::propagate(actor.skeleton);
    actor.bones = SkeletonPose::to_mesh_bones(actor.skeleton);
}
}
