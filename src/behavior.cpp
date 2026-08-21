#include "aethera/behavior.hpp"

namespace aethera {

void BehaviorSystem::add_rule(BehaviorRule rule) {
    rules_.push_back(std::move(rule));
}

void BehaviorSystem::emit(BehaviorEvent event, const BehaviorContext& context) {
    for (const auto& rule : rules_) {
        if (rule.event != event || !rule.action) continue;

        // Conditions are intentionally simple at this layer. An empty condition
        // always matches; the scripting layer will later compile richer expressions
        // into the same event system.
        if (rule.condition.empty() || rule.condition == context.state ||
            rule.condition == context.target) {
            rule.action(context);
        }
    }
}

void BehaviorSystem::set_flag(const std::string& name, bool value) {
    flags_[name] = value;
}

bool BehaviorSystem::flag(const std::string& name) const {
    const auto it = flags_.find(name);
    return it != flags_.end() && it->second;
}

const char* behavior_event_name(BehaviorEvent event) {
    switch (event) {
        case BehaviorEvent::Spawn: return "spawn";
        case BehaviorEvent::Update: return "update";
        case BehaviorEvent::EnterState: return "enter_state";
        case BehaviorEvent::LeaveState: return "leave_state";
        case BehaviorEvent::Collision: return "collision";
        case BehaviorEvent::NearObject: return "near_object";
        case BehaviorEvent::Damage: return "damage";
        case BehaviorEvent::Custom: return "custom";
        default: return "unknown";
    }
}

} // namespace aethera
