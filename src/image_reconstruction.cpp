#include "aethera/image_reconstruction.hpp"
#include "aethera/masked_image.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace aethera {
namespace {

Rect mask_bounds(const VisionMask& mask, int image_width, int image_height) {
    if (mask.width <= 0 || mask.height <= 0) return {};
    const std::size_t count = static_cast<std::size_t>(mask.width) * static_cast<std::size_t>(mask.height);
    if (mask.pixels.size() < count) return {};
    int min_x = mask.width, min_y = mask.height, max_x = -1, max_y = -1;
    const float threshold = std::clamp(mask.threshold, 0.0f, 1.0f) * 255.0f;
    for (int y = 0; y < mask.height; ++y) {
        for (int x = 0; x < mask.width; ++x) {
            if (mask.pixels[static_cast<std::size_t>(y * mask.width + x)] < threshold) continue;
            min_x = std::min(min_x, x); min_y = std::min(min_y, y);
            max_x = std::max(max_x, x); max_y = std::max(max_y, y);
        }
    }
    if (max_x < min_x || max_y < min_y) return {};
    const float sx = static_cast<float>(image_width) / static_cast<float>(mask.width);
    const float sy = static_cast<float>(image_height) / static_cast<float>(mask.height);
    return Rect{min_x * sx, min_y * sy, (max_x - min_x + 1) * sx, (max_y - min_y + 1) * sy};
}

std::size_t nearest_keypoint(const Rect& bounds, const std::vector<VisionKeypoint>& keypoints) {
    if (keypoints.empty()) return static_cast<std::size_t>(-1);
    const Vec2 center{bounds.x + bounds.w * 0.5f, bounds.y + bounds.h * 0.5f};
    std::size_t best = static_cast<std::size_t>(-1);
    float best_distance = std::numeric_limits<float>::max();
    for (std::size_t i = 0; i < keypoints.size(); ++i) {
        if (keypoints[i].confidence <= 0.0f) continue;
        const Vec2 p{keypoints[i].normalized_position.x * bounds.w + bounds.x,
                     keypoints[i].normalized_position.y * bounds.h + bounds.y};
        const float d = length(p - center);
        if (d < best_distance) { best_distance = d; best = i; }
    }
    return best;
}

std::string part_name(std::size_t index, const VisionKeypoint* keypoint) {
    if (keypoint != nullptr && !keypoint->name.empty()) return keypoint->name;
    return "part_" + std::to_string(index);
}

} // namespace

ImageReconstructionResult ImageReconstructor::reconstruct(const ImageRgba8& image,
                                                          const VisionModelOutput& output,
                                                          const VisionResult& semantic) const {
    ImageReconstructionResult result("reconstructed");
    result.object.set_image(&image);
    if (!image.valid()) { result.diagnostics.push_back("source image is invalid"); return result; }

    for (std::size_t i = 0; i < output.masks.size(); ++i) {
        const VisionMask& mask = output.masks[i];
        const Rect bounds = mask_bounds(mask, image.width, image.height);
        if (bounds.w <= 0.0f || bounds.h <= 0.0f) continue;
        const std::size_t keypoint = nearest_keypoint(bounds, output.keypoints);
        const VisionKeypoint* kp = keypoint == static_cast<std::size_t>(-1) ? nullptr : &output.keypoints[keypoint];

        int layer_x = 0, layer_y = 0;
        ImageRgba8 layer = extract_masked_layer(image, mask, layer_x, layer_y);
        if (!layer.valid()) { result.diagnostics.push_back("mask " + std::to_string(i) + " produced no layer"); continue; }

        ReconstructedPart part;
        part.name = part_name(i, kp); part.mask_index = i; part.anchor_keypoint = keypoint;
        part.bounds = Rect{static_cast<float>(layer_x), static_cast<float>(layer_y), static_cast<float>(layer.width), static_cast<float>(layer.height)};
        part.pivot = {0.5f, 0.5f};
        part.position = {part.bounds.x + part.bounds.w * 0.5f, part.bounds.y + part.bounds.h * 0.5f};
        part.confidence = kp != nullptr ? kp->confidence : 0.0f;
        result.parts.push_back(part);

        ImageNode node;
        node.name = part.name;
        node.visual.name = part.name;
        node.visual.source = part.bounds;
        node.visual.pivot = part.pivot;
        node.visual.asset = std::move(layer);
        node.local.position = part.position;
        node.visible = true;
        result.object.add_node(std::move(node));
    }
    if (!semantic.semantic.joints.empty()) result.diagnostics.push_back("semantic joints available for bone binding");
    result.object.update_world_transforms();
    return result;
}

} // namespace aethera
