#include <cassert>
#include <cmath>
#include "aethera/image_pipeline.hpp"
#include "aethera/animation_controller.hpp"
using namespace aethera;
int main() {
    ImageRgba8 image; image.width = 64; image.height = 96; image.pixels.resize(64u * 96u * 4u, 255);
    auto actor = ImagePipeline::build(image, 8, 12);
    assert(!actor.mesh.empty()); assert(!actor.indices.empty()); assert(actor.skeleton.size() == 2);
    const float original = actor.bones[1].current_position.y;
    AnimationController::sway(actor, 1.0f, 10.0f, 2.0f);
    assert(std::fabs(actor.bones[1].current_position.y - original) > 0.001f);
    return 0;
}
