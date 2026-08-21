#include "aethera/skinning.hpp"

#include <algorithm>
#include <cmath>

namespace aethera {

namespace {

constexpr std::size_t none = static_cast<std::size_t>(-1);

float safe_length(Vec2 v) {
    const float value = length(v);
    return value > 1e-5f ? value : 1.0f;
}

float angle_of(Vec2 v) {
    return std::atan2(v.y, v.x);
}

} // namespace

bool ImageSkinner::build(const SemanticObject& semantic,
                         const ImageAnalysisResult& analysis,
                         ImageObject& object) {
    bindings_.clear();
    if (semantic.joints.empty() || semantic.links.empty()) {
        return false;
    }

    object.update_world_transforms();

    for (const auto& link : semantic.links) {
        if (link.a >= semantic.joints.size() || link.b >= semantic.joints.size()) {
            continue;
        }

        const auto region_a = semantic.joints[link.a].source_region;
        const auto region_b = semantic.joints[link.b].source_region;
        if (region_a >= analysis.regions.size() || region_b >= analysis.regions.size()) {
            continue;
        }

        const Vec2 a = analysis.regions[region_a].centroid;
        const Vec2 b = analysis.regions[region_b].centroid;
        const float bind_length = safe_length(b - a);

        for (std::size_t node_index = 0; node_index < object.nodes().size(); ++node_index) {
            const ImageNode& node = object.nodes()[node_index];
            if (node_index >= semantic.regions.size()) {
                continue;
            }
            const std::size_t node_region = semantic.regions[node_index];
            if (node_region != region_b) {
                continue;
            }

            SkinBinding binding;
            binding.node = node_index;
            binding.joint_a = link.a;
            binding.joint_b = link.b;
            binding.bind_a = a;
            binding.bind_b = b;
            binding.width = std::max(0.01f, node.visual.source.height);
            bindings_.push_back(binding);
            break;
        }
    }

    return !bindings_.empty();
}

void ImageSkinner::apply_pose(const PhysicsWorld& physics,
                              ImageObject& object) const {
    if (physics.points().empty()) {
        return;
    }

    for (const auto& binding : bindings_) {
        if (binding.node >= object.nodes().size() ||
            binding.joint_a >= physics.points().size() ||
            binding.joint_b >= physics.points().size()) {
            continue;
        }

        const Vec2 current_a = physics.points()[binding.joint_a].position;
        const Vec2 current_b = physics.points()[binding.joint_b].position;
        const Vec2 current_axis = current_b - current_a;
        const Vec2 bind_axis = binding.bind_b - binding.bind_a;
        const float current_length = safe_length(current_axis);
        const float bind_length = safe_length(bind_axis);

        ImageNode& node = object.nodes()[binding.node];
        node.local.position = current_b;
        node.local.rotation = angle_of(current_axis) - angle_of(bind_axis);
        node.local.scale = {
            current_length / bind_length,
            1.0f
        };
    }

    object.update_world_transforms();
}

} // namespace aethera
