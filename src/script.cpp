#include "aethera/script.hpp"

#include <cctype>
#include <sstream>

namespace aethera {

namespace {

std::string trim(std::string value) {
    std::size_t first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) ++first;
    std::size_t last = value.size();
    while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) --last;
    return value.substr(first, last - first);
}

bool starts_with(const std::string& value, const std::string& prefix) {
    return value.rfind(prefix, 0) == 0;
}

void error(std::vector<ScriptError>& errors, std::size_t line, const std::string& message) {
    errors.push_back({line, message});
}

} // namespace

bool ScriptParser::parse(const std::string& source, ScriptProgram& program,
                         std::vector<ScriptError>& errors) const {
    program = {};
    errors.clear();
    ScriptRule* current_rule = nullptr;

    std::istringstream input(source);
    std::string raw;
    std::size_t line_number = 0;
    while (std::getline(input, raw)) {
        ++line_number;
        std::string line = trim(raw);
        if (line.empty() || line[0] == '#') continue;

        if (starts_with(line, "object ")) {
            program.object_name = trim(line.substr(7));
            current_rule = nullptr;
            if (program.object_name.empty()) error(errors, line_number, "object name is required");
            continue;
        }

        if (starts_with(line, "when ")) {
            const std::string trigger = trim(line.substr(5));
            if (trigger.empty()) {
                error(errors, line_number, "event name is required");
                current_rule = nullptr;
            } else {
                program.rules.push_back({trigger, {}, {}});
                current_rule = &program.rules.back();
            }
            continue;
        }

        if (starts_with(line, "if ")) {
            if (current_rule == nullptr) {
                error(errors, line_number, "condition must follow a when rule");
            } else {
                current_rule->condition = trim(line.substr(3));
            }
            continue;
        }

        if (current_rule == nullptr) {
            error(errors, line_number, "command must belong to a when rule");
            continue;
        }

        ScriptCommand command;
        if (starts_with(line, "set_state ")) {
            command.kind = ScriptCommandKind::SetState;
            command.argument = trim(line.substr(10));
        } else if (starts_with(line, "transition ")) {
            command.kind = ScriptCommandKind::Transition;
            command.argument = trim(line.substr(11));
        } else if (starts_with(line, "emit ")) {
            command.kind = ScriptCommandKind::EmitEvent;
            command.argument = trim(line.substr(5));
        } else if (starts_with(line, "set_flag ")) {
            command.kind = ScriptCommandKind::SetFlag;
            command.argument = trim(line.substr(9));
        } else if (starts_with(line, "clear_flag ")) {
            command.kind = ScriptCommandKind::ClearFlag;
            command.argument = trim(line.substr(11));
        } else if (starts_with(line, "wait ")) {
            command.kind = ScriptCommandKind::Wait;
            try {
                command.value = std::stof(trim(line.substr(5)));
            } catch (...) {
                error(errors, line_number, "wait expects a numeric duration");
                continue;
            }
        } else {
            error(errors, line_number, "unknown command: " + line);
            continue;
        }

        if (command.argument.empty() && command.kind != ScriptCommandKind::Wait) {
            error(errors, line_number, "command argument is required");
            continue;
        }
        if (command.kind == ScriptCommandKind::Wait && command.value < 0.0f) {
            error(errors, line_number, "wait duration cannot be negative");
            continue;
        }
        current_rule->commands.push_back(std::move(command));
    }

    if (program.object_name.empty()) {
        error(errors, 0, "script must declare an object");
    }
    return errors.empty();
}

const char* script_command_name(ScriptCommandKind kind) {
    switch (kind) {
        case ScriptCommandKind::SetState: return "set_state";
        case ScriptCommandKind::Transition: return "transition";
        case ScriptCommandKind::EmitEvent: return "emit";
        case ScriptCommandKind::SetFlag: return "set_flag";
        case ScriptCommandKind::ClearFlag: return "clear_flag";
        case ScriptCommandKind::Wait: return "wait";
        default: return "unknown";
    }
}

} // namespace aethera
