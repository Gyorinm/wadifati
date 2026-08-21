#include "aethera/skeleton_physics.hpp"

#include <algorithm>

namespace aethera {

bool SkeletonPhysics::build(const SemanticObject& semantic,
                            const ImageAnalysisResult& analysis,
                            PhysicsWorld& physics) {
    semantic_to_physics_.clear();
    semantic_to_physics_.resize(semantic.joints.size(), static_cast<std::size_t>(-1));

    if (semantic.joints.empty()) {
        return false;
    }

    for (std::size_t i = 0; i < semantic.joints.size(); ++i) {
        const auto& joint = semantic.joints[i];
        if (joint.source_region >= analysis.regions.size()) {
            continue;
        }

        BodyPoint point;
        point.position = analysis.regions[joint.source_region].centroid;
        point.previous_position = point.position;
        point.inverse_mass = (i == 0) ? 0.0f : 1.0f;
        point.pinned = (i == 0);

        semantic_to_physics_[i] = physics.points().size();
        physics.add_point(point);
    }

    for (const auto& link : semantic.links) {
        if (link.a >= semantic_to_physics_.size() ||
            link.b >= semantic_to_physics_.size()) {
            continue;
        }
        const auto a = semantic_to_physics_[link.a];
        const auto b = semantic_to_physics_[link.b];
        if (a == static_cast<std::size_t>(-1) || b == static_cast<std::size_t>(-1) ||
            a >= physics.points().size() || b >= physics.points().size()) {
            continue;
        }
        physics.add_constraint({a, b, link.rest_length, 0.9f});
    }

    return true;
}

void SkeletonPhysics::sync_to_semantic(const SemanticObject& semantic,
                                       const ImageAnalysisResult& analysis,
                                       PhysicsWorld& physics,
                                       float position_scale) {
    const std::size_t count = std::min(semantic.joints.size(), semantic_to_physics_.size());
    for (std::size_t i = 0; i < count; ++i) {
        const auto point_index = semantic_to_physics_[i];
        if (point_index == static_cast<std::size_t>(-1) || point_index >= physics.points().size()) {
            continue;
        }

        const auto region_index = semantic.joints[i].source_region;
        if (region_index >= analysis.regions.size()) {
            continue;
        }

        auto& point = physics.points()[point_index];
        if (!point.pinned) {
            continue;
        }

        const Vec2 target = analysis.regions[region_index].centroid * position_scale;
        point.position = target;
        point.previous_position = target;
    }
}

} // namespace aethera
