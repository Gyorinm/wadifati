#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace aethera {

enum class BehaviorEvent {
    Spawn,
    Update,
    EnterState,
    LeaveState,
    Collision,
    NearObject,
    Damage,
    Custom
};

struct BehaviorContext {
    float dt{0.0f};
    std::string state;
    std::string target;
    float value{0.0f};
};

using BehaviorAction = std::function<void(const BehaviorContext&)>;

struct BehaviorRule {
    BehaviorEvent event{BehaviorEvent::Update};
    std::string condition;
    BehaviorAction action;
};

class BehaviorSystem {
public:
    void add_rule(BehaviorRule rule);
    void emit(BehaviorEvent event, const BehaviorContext& context = {});
    void set_flag(const std::string& name, bool value);
    bool flag(const std::string& name) const;

private:
    std::vector<BehaviorRule> rules_;
    std::unordered_map<std::string, bool> flags_;
};

const char* behavior_event_name(BehaviorEvent event);

} // namespace aethera
