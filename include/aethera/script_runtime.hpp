#pragma once

#include "aethera/animation_graph.hpp"
#include "aethera/behavior.hpp"
#include "aethera/script.hpp"

namespace aethera {

class ScriptRuntime {
public:
    void load(const ScriptProgram& program) { program_ = program; }
    bool run_trigger(const std::string& trigger, AnimationGraph& animation,
                     BehaviorSystem& behavior) const;

private:
    ScriptProgram program_{};
};

} // namespace aethera
