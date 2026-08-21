#pragma once

#include <cmath>

#include "aethera/math.hpp"
#include "aethera/physics.hpp"

namespace aethera {

enum class MotionState {
    Idle,
    Walk,
    Run,
    Jump,
    Fall,
    Land
};

struct ProceduralPose {
    float time{0.0f};
    float cycle_speed{1.0f};
    float sway{2.0f};
    float stride{20.0f};
    float bounce{3.0f};
};

class ProceduralAnimator {
public:
    void set_state(MotionState state) { state_ = state; }
    MotionState state() const { return state_; }

    void update(float dt) { pose_.time += dt; }
    const ProceduralPose& pose() const { return pose_; }

    void apply_idle(PhysicsWorld& physics, std::size_t root, float amplitude = 2.0f) const;
    void apply_walk_cycle(PhysicsWorld& physics, std::size_t left_foot,
                          std::size_t right_foot, float amplitude = 18.0f) const;

private:
    MotionState state_{MotionState::Idle};
    ProceduralPose pose_{};
};

} // namespace aethera
