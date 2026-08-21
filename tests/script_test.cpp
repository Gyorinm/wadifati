#include "aethera/script.hpp"
#include "aethera/script_runtime.hpp"

#include <cassert>
#include <vector>

int main() {
    const char* source = R"(
object warrior
when spawn
    set_state idle
    set_flag alive
when alert
    if alive
    set_state walk
    emit noticed
    set_flag moving
when stop
    clear_flag moving
    set_state idle
)";

    aethera::ScriptParser parser;
    aethera::ScriptProgram program;
    std::vector<aethera::ScriptError> errors;
    const bool parsed = parser.parse(source, program, errors);
    assert(parsed);
    assert(errors.empty());
    assert(program.object_name == "warrior");
    assert(program.rules.size() == 3);

    aethera::AnimationGraph animation;
    aethera::BehaviorSystem behavior;
    aethera::ScriptRuntime runtime;
    runtime.load(program);

    assert(runtime.run_trigger("spawn", animation, behavior));
    assert(animation.current().state == aethera::AnimationState::Idle);
    assert(behavior.flag("alive"));

    assert(runtime.run_trigger("alert", animation, behavior));
    assert(animation.current().state == aethera::AnimationState::Walk);
    assert(behavior.flag("moving"));

    assert(runtime.run_trigger("stop", animation, behavior));
    assert(animation.current().state == aethera::AnimationState::Idle);
    assert(!behavior.flag("moving"));
    return 0;
}
