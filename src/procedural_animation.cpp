#include "aethera/procedural_animation.hpp"

namespace aethera {

void ProceduralAnimator::apply_idle(PhysicsWorld& physics, std::size_t root, float amplitude) const {
    if (root >= physics.points().size()) return;
    auto& point = physics.points()[root];
    if (!point.pinned) return;
    point.position.y += std::sin(pose_.time * pose_.cycle_speed * 2.0f) * amplitude;
    point.previous_position = point.position;
}

void ProceduralAnimator::apply_walk_cycle(PhysicsWorld& physics, std::size_t left_foot,
                                          std::size_t right_foot, float amplitude) const {
    auto& points = physics.points();
    if (left_foot >= points.size() || right_foot >= points.size()) return;

    const float phase = pose_.time * pose_.cycle_speed * 6.283185307f;
    const float left = std::sin(phase) * amplitude;
    const float right = std::sin(phase + 3.141592654f) * amplitude;

    points[left_foot].position.x += left * 0.05f;
    points[right_foot].position.x += right * 0.05f;
    points[left_foot].position.y -= std::max(0.0f, left) * 0.04f;
    points[right_foot].position.y -= std::max(0.0f, right) * 0.04f;
}

} // namespace aethera
