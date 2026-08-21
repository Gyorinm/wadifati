#include "aethera/script_runtime.hpp"

#include <cmath>
#include <sstream>

namespace aethera {
namespace {

void set_animation_state(AnimationGraph& animation, const std::string& name) {
    if (name == "idle") animation.set_state(AnimationState::Idle);
    else if (name == "walk") animation.set_state(AnimationState::Walk);
    else if (name == "run") animation.set_state(AnimationState::Run);
    else if (name == "jump") animation.set_state(AnimationState::Jump);
    else if (name == "fall") animation.set_state(AnimationState::Fall);
    else if (name == "land") animation.set_state(AnimationState::Land);
    else animation.set_state(AnimationState::Custom);
}

} // namespace

bool ScriptRuntime::run_trigger(const std::string& trigger,
                                AnimationGraph& animation,
                                BehaviorSystem& behavior) {
    return execute(trigger, animation, behavior, nullptr);
}

bool ScriptRuntime::run_trigger(const std::string& trigger,
                                AnimationGraph& animation,
                                BehaviorSystem& behavior,
                                PhysicsWorld& physics) {
    return execute(trigger, animation, behavior, &physics);
}

bool ScriptRuntime::execute(const std::string& trigger,
                            AnimationGraph& animation,
                            BehaviorSystem& behavior,
                            PhysicsWorld* physics) {
    bool executed = false;

    for (const auto& rule : program_.rules) {
        if (rule.trigger != trigger) continue;

        if (rule.condition.valid) {
            bool matches = false;
            if (!evaluate_expression(rule.condition, variables_, matches) || !matches) continue;
        }

        for (const auto& command : rule.commands) {
            switch (command.kind) {
                case ScriptCommandKind::SetState:
                    set_animation_state(animation, command.argument);
                    executed = true;
                    break;

                case ScriptCommandKind::Transition:
                    executed = animation.request(command.argument) || executed;
                    break;

                case ScriptCommandKind::EmitEvent:
                    behavior.emit(BehaviorEvent::Custom,
                                  BehaviorContext{command.argument, {}, command.argument});
                    executed = true;
                    break;

                case ScriptCommandKind::SetFlag:
                    behavior.set_flag(command.argument, true);
                    variables_[command.argument] = true;
                    executed = true;
                    break;

                case ScriptCommandKind::ClearFlag:
                    behavior.set_flag(command.argument, false);
                    variables_[command.argument] = false;
                    executed = true;
                    break;

                case ScriptCommandKind::SetVariable:
                    variables_[command.argument] = command.script_value;
                    if (const auto* value = std::get_if<bool>(&command.script_value)) {
                        behavior.set_flag(command.argument, *value);
                    }
                    executed = true;
                    break;

                case ScriptCommandKind::AddVariable: {
                    auto it = variables_.find(command.argument);
                    float value = 0.0f;
                    if (it != variables_.end()) {
                        if (const auto* number = std::get_if<float>(&it->second)) value = *number;
                    }
                    variables_[command.argument] = value + command.value;
                    executed = true;
                    break;
                }

                case ScriptCommandKind::Move:
                    if (physics != nullptr) executed = physics->move_point(0, command.vector) || executed;
                    break;

                case ScriptCommandKind::Rotate:
                    // Rotation is expressed as a state variable until ImageObject is passed
                    // into the runtime. Keeping it in variables makes scripts deterministic.
                    variables_["rotation"] = command.value;
                    executed = true;
                    break;

                case ScriptCommandKind::ApplyForce:
                    if (physics != nullptr) executed = physics->apply_force(0, command.vector) || executed;
                    break;

                case ScriptCommandKind::SetVelocity:
                    if (physics != nullptr) executed = physics->set_velocity(0, command.vector) || executed;
                    break;

                case ScriptCommandKind::Damage:
                    variables_["last_damage"] = command.value;
                    behavior.emit(BehaviorEvent::Damage,
                                  BehaviorContext{"", {}, "damage"});
                    executed = true;
                    break;

                case ScriptCommandKind::Call:
                    behavior.emit(BehaviorEvent::Custom,
                                  BehaviorContext{command.argument, {}, command.argument});
                    executed = true;
                    break;

                case ScriptCommandKind::Wait:
                    // Scheduler integration will consume wait commands. Parsing and
                    // runtime preservation are already supported.
                    executed = true;
                    break;
            }
        }
    }
    return executed;
}

} // namespace aethera
