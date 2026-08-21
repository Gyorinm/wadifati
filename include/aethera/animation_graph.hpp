#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace aethera {

enum class AnimationState {
    Idle,
    Walk,
    Run,
    Jump,
    Fall,
    Land,
    Custom
};

struct AnimationTransition {
    AnimationState from{AnimationState::Idle};
    AnimationState to{AnimationState::Idle};
    std::string condition;
    float blend_seconds{0.10f};
};

struct AnimationNode {
    AnimationState state{AnimationState::Idle};
    float time{0.0f};
    float speed{1.0f};
    bool looping{true};
};

class AnimationGraph {
public:
    void set_state(AnimationState state);
    AnimationState state() const { return current_.state; }

    void add_transition(AnimationTransition transition);
    bool request(const std::string& condition);
    void update(float dt);

    float state_time() const { return current_.time; }
    float blend_alpha() const { return blend_alpha_; }
    AnimationState previous_state() const { return previous_state_; }

private:
    AnimationNode current_{};
    AnimationState previous_state_{AnimationState::Idle};
    std::vector<AnimationTransition> transitions_;
    float blend_seconds_{0.0f};
    float blend_elapsed_{0.0f};
    float blend_alpha_{1.0f};
};

const char* animation_state_name(AnimationState state);

} // namespace aethera
