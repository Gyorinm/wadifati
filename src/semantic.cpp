#include "aethera/semantic.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>

namespace aethera {

namespace {

constexpr std::size_t none = static_cast<std::size_t>(-1);

float aspect(const ImageRegion& r) {
    return r.bounds.height > 0.0f ? r.bounds.width / r.bounds.height : 0.0f;
}

float area_ratio(const ImageRegion& r, const ImageAnalysisResult& a) {
    if (a.labels.empty()) return 0.0f;
    const float total = static_cast<float>(a.labels.size());
    return static_cast<float>(r.pixel_count) / total;
}

std::size_t largest_region(const ImageAnalysisResult& a) {
    std::size_t best = none;
    std::size_t best_count = 0;
    for (std::size_t i = 0; i < a.regions.size(); ++i) {
        if (a.regions[i].pixel_count > best_count) {
            best_count = a.regions[i].pixel_count;
            best = i;
        }
    }
    return best;
}

void link_nearest(SemanticObject& object, std::size_t anchor, const std::vector<std::size_t>& candidates) {
    if (anchor >= object.joints.size()) return;
    float best_distance = std::numeric_limits<float>::max();
    std::size_t best = none;
    for (const auto i : candidates) {
        if (i >= object.joints.size() || i == anchor) continue;
        const float d2 = length_squared(object.joints[i].position - object.joints[anchor].position);
        if (d2 < best_distance) {
            best_distance = d2;
            best = i;
        }
    }
    if (best != none) {
        object.links.push_back({anchor, best, std::sqrt(best_distance)});
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
    SemanticObject object;
    if (analysis.regions.empty()) return object;

    object.regions.resize(analysis.regions.size());
    for (std::size_t i = 0; i < analysis.regions.size(); ++i) object.regions[i] = i;

    const auto largest = largest_region(analysis);
    if (largest == none) return object;

    const auto& main = analysis.regions[largest];
    const float ar = aspect(main);
    const float ratio = area_ratio(main, analysis);
    const float count = static_cast<float>(analysis.regions.size());

    // These are intentionally conservative priors. They classify gross visual structure;
    // they do not claim to recognize arbitrary photographs semantically.
    if (count >= 6.0f && ar > 0.35f && ar < 1.8f) {
        object.kind = SemanticKind::Human;
        object.confidence = 0.35f;
    } else if (count >= 5.0f && ar > 1.7f && ratio < 0.45f) {
        object.kind = SemanticKind::Fish;
        object.confidence = 0.30f;
    } else if (count >= 7.0f && ar < 1.2f && ratio < 0.25f) {
        object.kind = SemanticKind::Insect;
        object.confidence = 0.25f;
    } else if (ar > 2.2f && ratio > 0.25f) {
        object.kind = SemanticKind::Landscape;
        object.confidence = 0.30f;
    } else if (ar > 1.4f && ratio > 0.18f) {
        object.kind = SemanticKind::Building;
        object.confidence = 0.25f;
    } else {
        object.kind = SemanticKind::Unknown;
        object.confidence = 0.10f;
    }

    return object;
}

void SemanticAnalyzer::infer_skeleton(SemanticObject& object) const {
    object.joints.clear();
    object.links.clear();
    if (object.regions.empty()) return;

    // The current skeleton stage operates on region centroids. A later AI segmenter can
    // supply richer landmarks without changing this representation.
    if (object.kind == SemanticKind::Human) {
        const std::size_t n = object.regions.size();
        const std::size_t anchor = 0;
        object.joints.push_back({"root", {}, object.regions[anchor]});

        std::vector<std::size_t> candidates;
        for (std::size_t i = 1; i < n; ++i) {
            candidates.push_back(i);
            object.joints.push_back({"part_" + std::to_string(i), {}, object.regions[i]});
        }
        link_nearest(object, 0, candidates);
        return;
    }

    // Generic fallback: connect regions as a minimum-style radial tree around the
    // largest component. This gives physics/animation a graph even for unknown objects.
    const std::size_t root_region = object.regions.front();
    object.joints.push_back({"root", {}, root_region});
    for (std::size_t i = 1; i < object.regions.size(); ++i) {
        object.joints.push_back({"part_" + std::to_string(i), {}, object.regions[i]});
    }
    std::vector<std::size_t> candidates;
    for (std::size_t i = 1; i < object.joints.size(); ++i) candidates.push_back(i);
    link_nearest(object, 0, candidates);
}

ImageObject SemanticAnalyzer::build_image_object(const ImageRgba8& image,
                                                  const ImageAnalysisResult& analysis,
                                                  const SemanticObject& semantic) const {
    ImageObject object("semantic_object");
    object.set_image(&image);

    for (std::size_t i = 0; i < semantic.regions.size(); ++i) {
        const std::size_t region_index = semantic.regions[i];
        if (region_index >= analysis.regions.size()) continue;

        const auto& region = analysis.regions[region_index];
        ImageNode node;
        node.name = "part_" + std::to_string(i);
        node.visual.name = node.name;
        node.visual.source = region.bounds;
        node.visual.pivot = {
            region.centroid.x / static_cast<float>(image.width),
            region.centroid.y / static_cast<float>(image.height)
        };
        node.local.position = region.centroid;
        object.add_node(std::move(node));
    }

    for (const auto& link : semantic.links) {
        if (link.a < object.nodes().size() && link.b < object.nodes().size()) {
            object.nodes()[link.b].parent = link.a;
            object.nodes()[link.b].local.position =
                analysis.regions[semantic.regions[link.b]].centroid -
                analysis.regions[semantic.regions[link.a]].centroid;
        }
    }

    object.update_world_transforms();
    return object;
}

} // namespace aethera
