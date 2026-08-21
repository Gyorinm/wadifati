#include "aethera/vision_model.hpp"

#include <cassert>

int main() {
    aethera::ImageRgba8 image;
    image.width = 100;
    image.height = 50;
    image.pixels.resize(100u * 50u * 4u, 255);

    aethera::VisionModelOutput output;
    output.semantic_scores = std::vector<float>(12, 0.0f);
    output.semantic_scores[1] = 0.91f;
    output.keypoints.push_back({"head", {0.5f, 0.2f}, 0.95f});
    output.keypoints.push_back({"torso", {0.5f, 0.6f}, 0.90f});

    aethera::VisionMask mask;
    mask.width = 100;
    mask.height = 50;
    mask.pixels.assign(100u * 50u, 0);
    mask.pixels[10u * 100u + 10u] = 255;
    output.masks.push_back(mask);

    auto result = aethera::merge_model_output(image, output);
    assert(result.semantic.kind == aethera::SemanticKind::Human);
    assert(result.semantic.confidence > 0.9f);
    assert(result.semantic.joints.size() == 2);
    assert(result.semantic.links.size() == 1);
    assert(result.analysis.regions.size() == 1);
    assert(result.semantic.joints[0].position.x == 50.0f);
    assert(result.semantic.joints[0].position.y == 10.0f);
    return 0;
}
