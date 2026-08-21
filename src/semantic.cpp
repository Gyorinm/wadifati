#include "aethera/semantic.hpp"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace aethera {

namespace {

constexpr std::size_t none = static_cast<std::size_t>(-1);

float aspect(const ImageRegion& r) {
    return r.bounds.height > 0.0f ? r.bounds.width / r.bounds.height : 0.0f;
}

float area_ratio(const ImageRegion& r, const ImageAnalysisResult& a) {
    if (a.labels.empty()) return 0.0f;
    return static_cast<float>(r.pixel_count) / static_cast<float>(a.labels.size());
}

std::size_t largest_region(const ImageAnalysisResult& a) {
    std::size_t best = none;
    std::size_t count = 0;
    for (std::size_t i = 0; i < a.regions.size(); ++i) {
        if (a.regions[i].pixel_count > count) {
            count = a.regions[i].pixel_count;
            best = i;
        }
    }
    return best;
}

void add_nearest_links(SemanticObject& object) {
    if (object.joints.size() < 2) return;

    // Connect every joint to its nearest earlier joint. This gives a deterministic
    // tree without requiring a heavyweight graph library.
    for (std::size_t i = 1; i < object.joints.size(); ++i) {
        float best_d2 = std::numeric_limits<float>::max();
        std::size_t best = 0;
        for (std::size_t j = 0; j < i; ++j) {
            const float d2 = length_squared(object.joints[i].position - object.joints[j].position);
            if (d2 < best_d2) {
                best_d2 = d2;
                best = j;
            }
        }
        object.links.push_back({best, i, std::sqrt(best_d2)});
    }
}

} // namespace

const char* semantic_kind_name(SemanticKind kind) {
    switch (kind) {
        case SemanticKind::Human: return "human";
        case SemanticKind::Quadruped: return "quadruped";
        case SemanticKind::Insect: return "insect";
        case SemanticKind::Fish: return "fish";
        case SemanticKind::Amphibian: return "amphibian";
        case SemanticKind::Plant: return "plant";
        case SemanticKind::Tree: return "tree";
        case SemanticKind::Building: return "building";
        case SemanticKind::Vehicle: return "vehicle";
        case SemanticKind::Landscape: return "landscape";
        case SemanticKind::Water: return "water";
        default: return "unknown";
    }
}

SemanticObject SemanticAnalyzer::classify(const ImageAnalysisResult& analysis) const {
    SemanticObject result;
    if (analysis.regions.empty()) return result;

    const std::size_t largest = largest_region(analysis);
    if (largest == none) return result;

    result.regions.resize(analysis.regions.size());
    for (std::size_t i = 0; i < result.regions.size(); ++i) result.regions[i] = i;

    const ImageRegion& main = analysis.regions[largest];
    const float ar = aspect(main);
    const float ratio = area_ratio(main, analysis);
    const float count = static_cast<float>(analysis.regions.size());

    if (count >= 6.0f && ar > 0.35f && ar < 1.8f) {
        result.kind = SemanticKind::Human;
        result.confidence = 0.35f;
    } else if (count >= 5.0f && ar > 1.7f && ratio < 0.45f) {
        result.kind = SemanticKind::Fish;
        result.confidence = 0.30f;
    } else if (count >= 7.0f && ar < 1.2f && ratio < 0.25f) {
        result.kind = SemanticKind::Insect;
        result.confidence = 0.25f;
    } else if (ar > 2.2f && ratio > 0.25f) {
        result.kind = SemanticKind::Landscape;
        result.confidence = 0.30f;
    } else if (ar > 1.4f && ratio > 0.18f) {
        result.kind = SemanticKind::Building;
        result.confidence = 0.25f;
    }

    return result;
}

void SemanticAnalyzer::infer_skeleton(const ImageAnalysisResult& analysis,
                                      SemanticObject& object) const {
    object.joints.clear();
    object.links.clear();
    if (object.regions.empty()) return;

    // Region centroids are the first landmark source. A future AI landmark provider
    // can populate the same representation with anatomically meaningful points.
    for (std::size_t i = 0; i < object.regions.size(); ++i) {
        const std::size_t region_index = object.regions[i];
        if (region_index >= analysis.regions.size()) continue;
        const Vec2 center = analysis.regions[region_index].centroid;
        std::string name = "part_" + std::to_string(i);
        if (i == 0) name = "root";
        object.joints.push_back({std::move(name), center, region_index});
    }

    add_nearest_links(object);
}

ImageObject SemanticAnalyzer::build_image_object(const ImageRgba8& image,
                                                 const ImageAnalysisResult& analysis,
                                                 const SemanticObject& semantic) const {
    ImageObject object("semantic_object");
    object.set_image(&image);

    for (std::size_t i = 0; i < semantic.joints.size(); ++i) {
        const auto& joint = semantic.joints[i];
        if (joint.source_region >= analysis.regions.size()) continue;

        const auto& region = analysis.regions[joint.source_region];
        ImageNode node;
        node.name = joint.name;
        node.visual.name = joint.name;
        node.visual.source = region.bounds;
        node.visual.pivot = {
            region.centroid.x / static_cast<float>(image.width),
            region.centroid.y / static_cast<float>(image.height)
        };
        node.local.position = joint.position;
        object.add_node(std::move(node));
    }

    for (const auto& link : semantic.links) {
        if (link.a >= object.nodes().size() || link.b >= object.nodes().size()) continue;
        object.nodes()[link.b].parent = link.a;
        object.nodes()[link.b].local.position =
            semantic.joints[link.b].position - semantic.joints[link.a].position;
    }

    object.update_world_transforms();
    return object;
}

} // namespace aethera
