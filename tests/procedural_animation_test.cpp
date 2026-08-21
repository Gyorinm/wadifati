#include "aethera/procedural_animation.hpp"

#include <cassert>

int main() {
    aethera::PhysicsWorld physics;
    physics.add_point({{100.0f, 100.0f}, {100.0f, 100.0f}, 0.0f, true});
    physics.add_point({{80.0f, 200.0f}, {80.0f, 200.0f}, 1.0f, false});
    physics.add_point({{120.0f, 200.0f}, {120.0f, 200.0f}, 1.0f, false});

    aethera::ProceduralAnimator animator;
    animator.set_state(aethera::MotionState::Walk);
    assert(animator.state() == aethera::MotionState::Walk);
    animator.update(0.25f);
    const float left_before = physics.points()[1].position.x;
    const float right_before = physics.points()[2].position.x;
    animator.apply_walk_cycle(physics, 1, 2);
    assert(physics.points()[1].position.x != left_before || physics.points()[2].position.x != right_before);
    return 0;
}
