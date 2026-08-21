#include "aethera/image_reconstruction.hpp"

#include <cassert>

int main() {
    aethera::ImageRgba8 image;
    image.width = 4;
    image.height = 4;
    image.pixels.resize(4 * 4 * 4, 255);

    aethera::VisionModelOutput output;
    aethera::VisionMask mask;
    mask.width = 4;
    mask.height = 4;
    mask.pixels.assign(16, 0);
    mask.pixels[5] = 255;
    mask.pixels[6] = 255;
    mask.pixels[9] = 255;
    mask.pixels[10] = 255;
    output.masks.push_back(mask);
    output.keypoints.push_back({"center", {0.5f, 0.5f}, 0.95f});

    aethera::ImageReconstructor reconstructor;
    auto result = reconstructor.reconstruct(image, output);

    assert(result.object.image() == &image);
    assert(result.parts.size() == 1);
    assert(result.object.nodes().size() == 1);
    assert(result.parts[0].anchor_keypoint == 0);
    assert(result.parts[0].bounds.w > 0.0f);
    assert(result.parts[0].bounds.h > 0.0f);
    assert(result.object.nodes()[0].visual.source.w > 0.0f);
    return 0;
}
