#include "aethera/script_runtime.hpp"

namespace aethera {

bool ScriptRuntime::run_trigger(const std::string& trigger, AnimationGraph& animation,
                               BehaviorSystem& behavior) const {
    bool executed = false;
    for (const auto& rule : program_.rules) {
        if (rule.trigger != trigger) continue;

        if (!rule.condition.empty() && !behavior.flag(rule.condition)) continue;

        for (const auto& command : rule.commands) {
            switch (command.kind) {
                case ScriptCommandKind::SetState:
                    if (command.argument == "idle") animation.set_state(AnimationState::Idle);
                    else if (command.argument == "walk") animation.set_state(AnimationState::Walk);
                    else if (command.argument == "run") animation.set_state(AnimationState::Run);
                    else if (command.argument == "jump") animation.set_state(AnimationState::Jump);
                    else if (command.argument == "fall") animation.set_state(AnimationState::Fall);
                    else if (command.argument == "land") animation.set_state(AnimationState::Land);
                    else animation.set_state(AnimationState::Custom);
                    executed = true;
                    break;
                case ScriptCommandKind::Transition:
                    executed = animation.request(command.argument) || executed;
                    break;
                case ScriptCommandKind::EmitEvent:
                    behavior.emit(BehaviorEvent::Custom, BehaviorContext{command.argument, {}, command.argument});
                    executed = true;
                    break;
                case ScriptCommandKind::SetFlag:
                    behavior.set_flag(command.argument, true);
                    executed = true;
                    break;
                case ScriptCommandKind::ClearFlag:
                    behavior.set_flag(command.argument, false);
                    executed = true;
                    break;
                case ScriptCommandKind::Wait:
                    // Timing is represented by the script command now; a scheduler will
                    // consume wait nodes later without changing the language format.
                    executed = true;
                    break;
            }
        }
    }
    return executed;
}

} // namespace aethera
