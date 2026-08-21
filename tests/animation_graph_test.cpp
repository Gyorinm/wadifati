#include "aethera/animation_graph.hpp"

#include <cassert>

int main() {
    aethera::AnimationGraph graph;
    graph.add_transition({aethera::AnimationState::Idle, aethera::AnimationState::Walk, "move", 0.2f});
    graph.add_transition({aethera::AnimationState::Walk, aethera::AnimationState::Run, "sprint", 0.1f});

    assert(graph.state() == aethera::AnimationState::Idle);
    assert(graph.request("move"));
    assert(graph.state() == aethera::AnimationState::Walk);
    assert(graph.previous_state() == aethera::AnimationState::Idle);
    graph.update(0.1f);
    assert(graph.state_time() > 0.0f);
    assert(graph.blend_alpha() > 0.0f && graph.blend_alpha() < 1.0f);
    graph.update(0.2f);
    assert(graph.blend_alpha() == 1.0f);
    assert(graph.request("sprint"));
    assert(graph.state() == aethera::AnimationState::Run);
    return 0;
}
