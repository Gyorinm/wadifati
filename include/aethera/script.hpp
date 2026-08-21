#pragma once

#include <string>
#include <variant>
#include <vector>

namespace aethera {

enum class ScriptCommandKind {
    SetState,
    Transition,
    EmitEvent,
    SetFlag,
    ClearFlag,
    Wait
};

struct ScriptCommand {
    ScriptCommandKind kind{ScriptCommandKind::Wait};
    std::string argument;
    float value{0.0f};
};

struct ScriptRule {
    std::string trigger;
    std::string condition;
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
