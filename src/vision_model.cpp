#include "aethera/vision_model.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace aethera {

const char* vision_semantic_class_name(std::size_t index) {
    switch (index) {
        case 0: return "unknown";
        case 1: return "human";
        case 2: return "quadruped";
        case 3: return "insect";
        case 4: return "fish";
        case 5: return "amphibian";
        case 6: return "plant";
        case 7: return "tree";
        case 8: return "building";
        case 9: return "vehicle";
        case 10: return "landscape";
        case 11: return "water";
        default: return "unknown";
    }
}

SemanticKind vision_semantic_kind(std::size_t index) {
    switch (index) {
        case 1: return SemanticKind::Human;
        case 2: return SemanticKind::Quadruped;
        case 3: return SemanticKind::Insect;
        case 4: return SemanticKind::Fish;
        case 5: return SemanticKind::Amphibian;
        case 6: return SemanticKind::Plant;
        case 7: return SemanticKind::Tree;
        case 8: return SemanticKind::Building;
        case 9: return SemanticKind::Vehicle;
        case 10: return SemanticKind::Landscape;
        case 11: return SemanticKind::Water;
        default: return SemanticKind::Unknown;
    }
}

VisionResult merge_model_output(const ImageRgba8& image,
                                const VisionModelOutput& output,
                                const VisionOptions& options) {
    VisionResult result;
    ImageAnalysisResult analysis;
    analysis.labels.assign(static_cast<std::size_t>(std::max(0, image.width * image.height)), -1);

    for (const auto& mask : output.masks) {
        if (mask.width != image.width || mask.height != image.height) {
            result.diagnostics.push_back("ignored mask with incompatible dimensions");
            continue;
        }
        ImageRegion region;
        region.kind = ImageObjectKind::Region;
        std::size_t count = 0;
        double sx = 0.0;
        double sy = 0.0;
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                const auto idx = static_cast<std::size_t>(y * image.width + x);
                if (idx >= mask.pixels.size() || mask.pixels[idx] == 0) continue;
                ++count;
                sx += x;
                sy += y;
            }
        }
        if (count == 0) continue;
        region.pixel_count = count;
        region.centroid = {static_cast<float>(sx / count), static_cast<float>(sy / count)};
        region.bounds = Rect{0.0f, 0.0f, static_cast<float>(image.width), static_cast<float>(image.height)};
        analysis.regions.push_back(region);
    }

    result.analysis = std::move(analysis);

    if (!output.semantic_scores.empty()) {
        std::size_t best = 0;
        float best_score = -std::numeric_limits<float>::infinity();
        for (std::size_t i = 0; i < output.semantic_scores.size(); ++i) {
            if (output.semantic_scores[i] > best_score) {
                best = i;
                best_score = output.semantic_scores[i];
            }
        }
        result.semantic.kind = vision_semantic_kind(best);
        result.semantic.confidence = std::clamp(best_score, 0.0f, 1.0f);
    }

    if (options.infer_skeleton) {
        for (std::size_t i = 0; i < output.keypoints.size(); ++i) {
            const auto& point = output.keypoints[i];
            if (point.confidence <= 0.0f) continue;
            SemanticJoint joint;
            joint.name = point.name.empty() ? ("keypoint_" + std::to_string(i)) : point.name;
            joint.position = {
                point.normalized_position.x * static_cast<float>(image.width),
                point.normalized_position.y * static_cast<float>(image.height)
            };
            result.semantic.joints.push_back(joint);
        }
        for (std::size_t i = 1; i < result.semantic.joints.size(); ++i) {
            const auto& a = result.semantic.joints[i - 1];
            const auto& b = result.semantic.joints[i];
            result.semantic.links.push_back({i - 1, i, length(b.position - a.position)});
        }
    }

    result.diagnostics.push_back("external model output normalized");
    return result;
}

} // namespace aethera
