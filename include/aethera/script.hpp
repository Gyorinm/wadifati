#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <vector>

#include "aethera/math.hpp"
#include "aethera/script_expr.hpp"

namespace aethera {

enum class ScriptCommandKind {
    SetState,
    Transition,
    EmitEvent,
    SetFlag,
    ClearFlag,
    SetVariable,
    AddVariable,
    Move,
    Rotate,
    ApplyForce,
    SetVelocity,
    Damage,
    RotatePart,
    MovePart,
    Reach,
    Call,
    Wait
};

struct ScriptCommand {
    ScriptCommandKind kind{ScriptCommandKind::Wait};
    std::string argument;
    float value{0.0f};
    Vec2 vector{};
    std::vector<float> args;
    ScriptValue script_value{false};
};

struct ScriptRule {
    std::string trigger;
    ScriptExpression condition{};
    std::vector<ScriptCommand> commands;
};

struct ScriptProgram {
    std::string object_name;
    std::vector<ScriptRule> rules;
};

struct ScriptError {
    std::size_t line{0};
    std::string message;
};

class ScriptParser {
public:
    bool parse(const std::string& source, ScriptProgram& program,
               std::vector<ScriptError>& errors) const;
};

const char* script_command_name(ScriptCommandKind kind);

} // namespace aethera
