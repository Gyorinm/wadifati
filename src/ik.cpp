#include "aethera/ik.hpp"

#include <algorithm>
#include <cmath>

namespace aethera {

namespace {

float cross(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }

} // namespace

bool IK2D::solve(IKChain2D chain, PhysicsWorld& physics, Vec2 target, float tolerance) {
    auto& points = physics.points();
    if (chain.root >= points.size() || chain.joint >= points.size() || chain.end >= points.size()) {
        return false;
    }
    if (chain.root == chain.joint || chain.joint == chain.end || chain.root == chain.end) {
        return false;
    }

    if (chain.root_to_joint <= 0.0f) {
        chain.root_to_joint = length(points[chain.joint].position - points[chain.root].position);
    }
    if (chain.joint_to_end <= 0.0f) {
        chain.joint_to_end = length(points[chain.end].position - points[chain.joint].position);
    }

    const float max_reach = chain.root_to_joint + chain.joint_to_end;
    const float min_reach = std::fabs(chain.root_to_joint - chain.joint_to_end);
    Vec2 root = points[chain.root].position;
    Vec2 desired = target - root;
    const float desired_distance = length(desired);
    if (desired_distance < 1e-5f) {
        return false;
    }

    const float clamped_distance = clamp(desired_distance, min_reach, max_reach);
    const float base_angle = std::atan2(desired.y, desired.x);

    const float cos_root = clamp(
        (chain.root_to_joint * chain.root_to_joint + clamped_distance * clamped_distance -
         chain.joint_to_end * chain.joint_to_end) /
            (2.0f * chain.root_to_joint * clamped_distance),
        -1.0f, 1.0f);

    const float joint_offset = std::acos(cos_root);
    const Vec2 target_dir{std::cos(base_angle), std::sin(base_angle)};
    const Vec2 side{-target_dir.y, target_dir.x};

    const Vec2 candidate_a = root + (target_dir * std::cos(joint_offset) +
                                     side * std::sin(joint_offset)) * chain.root_to_joint;
    const Vec2 candidate_b = root + (target_dir * std::cos(joint_offset) -
                                     side * std::sin(joint_offset)) * chain.root_to_joint;

    const Vec2 current_joint = points[chain.joint].position;
    const Vec2 candidate =
        length_squared(candidate_a - current_joint) <= length_squared(candidate_b - current_joint)
            ? candidate_a
            : candidate_b;

    const int iterations = std::max(1, static_cast<int>(chain.max_iterations));
    Vec2 joint = candidate;
    Vec2 end = root + normalized(target - root) * clamped_distance;

    for (int i = 0; i < iterations; ++i) {
        const Vec2 end_to_target = target - end;
        if (length(end_to_target) <= tolerance) break;

        joint = root + normalized(end - root) * chain.root_to_joint;
        end = target;
    }

    if (!points[chain.root].pinned) {
        points[chain.root].position = root;
    }
    points[chain.joint].position = joint;
    points[chain.end].position = end;
    points[chain.joint].previous_position = joint;
    points[chain.end].previous_position = end;
    return true;
}

} // namespace aethera
