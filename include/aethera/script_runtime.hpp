#pragma once

#include <unordered_map>

#include "aethera/animation_graph.hpp"
#include "aethera/behavior.hpp"
#include "aethera/ik.hpp"
#include "aethera/image_object.hpp"
#include "aethera/physics.hpp"
#include "aethera/script.hpp"

namespace aethera {

class ScriptRuntime {
public:
    void load(const ScriptProgram& program) { program_ = program; }
    bool run_trigger(const std::string& trigger, AnimationGraph& animation, BehaviorSystem& behavior);
    bool run_trigger(const std::string& trigger, AnimationGraph& animation, BehaviorSystem& behavior, PhysicsWorld& physics);
    bool run_trigger(const std::string& trigger, AnimationGraph& animation, BehaviorSystem& behavior,
                     PhysicsWorld& physics, ImageObject& image);

    void set_variable(const std::string& name, ScriptValue value) { variables_[name] = std::move(value); }
    const std::unordered_map<std::string, ScriptValue>& variables() const { return variables_; }

private:
    bool execute(const std::string& trigger, AnimationGraph& animation, BehaviorSystem& behavior,
                 PhysicsWorld* physics, ImageObject* image);

    ScriptProgram program_{};
    std::unordered_map<std::string, ScriptValue> variables_;
};

} // namespace aethera
