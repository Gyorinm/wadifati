#include "aethera/animation_graph.hpp"

#include <algorithm>

namespace aethera {

void AnimationGraph::set_state(AnimationState state) {
    if (current_.state == state) return;
    previous_state_ = current_.state;
    current_.state = state;
    current_.time = 0.0f;
}

void AnimationGraph::add_transition(AnimationTransition transition) {
    transitions_.push_back(std::move(transition));
}

bool AnimationGraph::request(const std::string& condition) {
    for (const auto& transition : transitions_) {
        if (transition.from == current_.state && transition.condition == condition) {
            previous_state_ = current_.state;
            current_.state = transition.to;
            current_.time = 0.0f;
            blend_seconds_ = std::max(0.0f, transition.blend_seconds);
            blend_elapsed_ = 0.0f;
            blend_alpha_ = blend_seconds_ > 0.0f ? 0.0f : 1.0f;
            return true;
        }
    }
    return false;
}

void AnimationGraph::update(float dt) {
    dt = std::max(0.0f, dt);
    current_.time += dt * current_.speed;
    if (blend_seconds_ > 0.0f) {
        blend_elapsed_ += dt;
        blend_alpha_ = std::clamp(blend_elapsed_ / blend_seconds_, 0.0f, 1.0f);
        if (blend_alpha_ >= 1.0f) blend_seconds_ = 0.0f;
    }
}

const char* animation_state_name(AnimationState state) {
    switch (state) {
        case AnimationState::Idle: return "idle";
        case AnimationState::Walk: return "walk";
        case AnimationState::Run: return "run";
        case AnimationState::Jump: return "jump";
        case AnimationState::Fall: return "fall";
        case AnimationState::Land: return "land";
        case AnimationState::Custom: return "custom";
        default: return "unknown";
    }
}

} // namespace aethera
