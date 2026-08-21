#pragma once

#include <unordered_map>

#include "aethera/animation_graph.hpp"
#include "aethera/behavior.hpp"
#include "aethera/script.hpp"

namespace aethera {

class ScriptRuntime {
public:
    void load(const ScriptProgram& program) { program_ = program; }
    bool run_trigger(const std::string& trigger, AnimationGraph& animation,
                     BehaviorSystem& behavior);
    void set_variable(const std::string& name, ScriptValue value) { variables_[name] = std::move(value); }
    const std::unordered_map<std::string, ScriptValue>& variables() const { return variables_; }

private:
    ScriptProgram program_{};
    std::unordered_map<std::string, ScriptValue> variables_;
};

} // namespace aethera
