#include "aethera/animation_graph.hpp"
#include "aethera/behavior.hpp"
#include "aethera/physics.hpp"
#include "aethera/script.hpp"
#include "aethera/script_runtime.hpp"

#include <cassert>
#include <cmath>
#include <vector>

int main() {
    const char* source = R"(
object fighter
when launch
    set power 10
    move 5 0
    set_velocity 20 0
    apply_force 10 0
    set_state run
    damage 7
)
)";

    aethera::ScriptParser parser;
    aethera::ScriptProgram program;
    std::vector<aethera::ScriptError> errors;
    assert(parser.parse(source, program, errors));
    assert(errors.empty());

    aethera::AnimationGraph animation;
    aethera::BehaviorSystem behavior;
    aethera::PhysicsWorld physics;
    physics.gravity = {};

    aethera::BodyPoint point;
    point.position = {10.0f, 20.0f};
    point.previous_position = point.position;
    physics.add_point(point);

    aethera::ScriptRuntime runtime;
    runtime.load(program);
    assert(runtime.run_trigger("launch", animation, behavior, physics));

    assert(std::abs(physics.points()[0].position.x - 15.0f) < 1e-4f);
    assert(std::abs(physics.velocity(0).x - 20.0f) < 1e-4f);
    assert(animation.current().state == aethera::AnimationState::Run);
    const auto it = runtime.variables().find("last_damage");
    assert(it != runtime.variables().end());
    assert(std::abs(std::get<float>(it->second) - 7.0f) < 1e-4f);
    return 0;
}
