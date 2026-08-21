#include "aethera/script.hpp"

#include <cctype>
#include <sstream>

namespace aethera {
namespace {
std::string trim(std::string value) {
    std::size_t a = 0, b = value.size();
    while (a < b && std::isspace(static_cast<unsigned char>(value[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(value[b - 1]))) --b;
    return value.substr(a, b - a);
}

bool starts_with(const std::string& value, const std::string& prefix) {
    return value.rfind(prefix, 0) == 0;
}

void error(std::vector<ScriptError>& errors, std::size_t line, const std::string& message) {
    errors.push_back({line, message});
}

bool parse_vec2(const std::string& raw, Vec2& out) {
    std::istringstream stream(raw);
    return static_cast<bool>(stream >> out.x >> out.y);
}

bool parse_single_float(const std::string& raw, float& out) {
    std::istringstream stream(raw);
    return static_cast<bool>(stream >> out);
}
}

bool ScriptParser::parse(const std::string& source, ScriptProgram& program,
                         std::vector<ScriptError>& errors) const {
    program = {};
    errors.clear();
    ScriptRule* rule = nullptr;

    std::istringstream input(source);
    std::string raw;
    std::size_t line = 0;
    while (std::getline(input, raw)) {
        ++line;
        const std::string s = trim(raw);
        if (s.empty() || s[0] == '#') continue;

        if (starts_with(s, "object ")) {
            program.object_name = trim(s.substr(7));
            rule = nullptr;
            if (program.object_name.empty()) error(errors, line, "object name is required");
            continue;
        }

        if (starts_with(s, "when ")) {
            const std::string trigger = trim(s.substr(5));
            if (trigger.empty()) {
                error(errors, line, "event name is required");
                rule = nullptr;
            } else {
                program.rules.push_back({trigger, {}, {}});
                rule = &program.rules.back();
            }
            continue;
        }

        if (starts_with(s, "if ")) {
            if (!rule) {
                error(errors, line, "condition must follow a when rule");
            } else if (!parse_expression(trim(s.substr(3)), rule->condition)) {
                error(errors, line, "invalid condition");
            }
            continue;
        }

        if (!rule) {
            error(errors, line, "command must belong to a when rule");
            continue;
        }

        ScriptCommand command;
        std::string rest;

        if (starts_with(s, "set_state ")) {
            command.kind = ScriptCommandKind::SetState;
            rest = trim(s.substr(10));
        } else if (starts_with(s, "transition ")) {
            command.kind = ScriptCommandKind::Transition;
            rest = trim(s.substr(11));
        } else if (starts_with(s, "emit ")) {
            command.kind = ScriptCommandKind::EmitEvent;
            rest = trim(s.substr(5));
        } else if (starts_with(s, "set_flag ")) {
            command.kind = ScriptCommandKind::SetFlag;
            rest = trim(s.substr(9));
        } else if (starts_with(s, "clear_flag ")) {
            command.kind = ScriptCommandKind::ClearFlag;
            rest = trim(s.substr(11));
        } else if (starts_with(s, "set ")) {
            command.kind = ScriptCommandKind::SetVariable;
            std::istringstream stream(s.substr(4));
            stream >> command.argument;
            std::string value_text;
            std::getline(stream, value_text);
            value_text = trim(value_text);
            if (command.argument.empty() || value_text.empty()) {
                error(errors, line, "set expects a variable and value");
                continue;
            }
            if (!parse_script_value(value_text, command.script_value)) {
                error(errors, line, "invalid value");
                continue;
            }
        } else if (starts_with(s, "add ")) {
            command.kind = ScriptCommandKind::AddVariable;
            std::istringstream stream(s.substr(4));
            stream >> command.argument >> command.value;
            if (command.argument.empty() || stream.fail()) {
                error(errors, line, "add expects variable and numeric value");
                continue;
            }
        } else if (starts_with(s, "move ")) {
            command.kind = ScriptCommandKind::Move;
            rest = trim(s.substr(5));
            if (!parse_vec2(rest, command.vector)) {
                error(errors, line, "move expects dx dy");
                continue;
            }
        } else if (starts_with(s, "rotate ")) {
            command.kind = ScriptCommandKind::Rotate;
            rest = trim(s.substr(7));
            if (!parse_single_float(rest, command.value)) {
                error(errors, line, "rotate expects degrees");
                continue;
            }
        } else if (starts_with(s, "apply_force ")) {
            command.kind = ScriptCommandKind::ApplyForce;
            rest = trim(s.substr(12));
            if (!parse_vec2(rest, command.vector)) {
                error(errors, line, "apply_force expects fx fy");
                continue;
            }
        } else if (starts_with(s, "set_velocity ")) {
            command.kind = ScriptCommandKind::SetVelocity;
            rest = trim(s.substr(13));
            if (!parse_vec2(rest, command.vector)) {
                error(errors, line, "set_velocity expects vx vy");
                continue;
            }
        } else if (starts_with(s, "damage ")) {
            command.kind = ScriptCommandKind::Damage;
            rest = trim(s.substr(7));
            if (!parse_single_float(rest, command.value) || command.value < 0.0f) {
                error(errors, line, "damage expects a non-negative number");
                continue;
            }
        } else if (starts_with(s, "call ")) {
            command.kind = ScriptCommandKind::Call;
            rest = trim(s.substr(5));
        } else if (starts_with(s, "wait ")) {
            command.kind = ScriptCommandKind::Wait;
            try {
                command.value = std::stof(trim(s.substr(5)));
            } catch (...) {
                error(errors, line, "wait expects a numeric duration");
                continue;
            }
            if (command.value < 0.0f) {
                error(errors, line, "wait duration cannot be negative");
                continue;
            }
        } else {
            error(errors, line, "unknown command: " + s);
            continue;
        }

        if ((command.kind == ScriptCommandKind::SetState ||
             command.kind == ScriptCommandKind::Transition ||
             command.kind == ScriptCommandKind::EmitEvent ||
             command.kind == ScriptCommandKind::SetFlag ||
             command.kind == ScriptCommandKind::ClearFlag ||
             command.kind == ScriptCommandKind::Call) && rest.empty()) {
            error(errors, line, "command argument is required");
            continue;
        }
        if (!rest.empty() && command.kind != ScriptCommandKind::Move &&
            command.kind != ScriptCommandKind::Rotate &&
            command.kind != ScriptCommandKind::ApplyForce &&
            command.kind != ScriptCommandKind::SetVelocity &&
            command.kind != ScriptCommandKind::Damage) {
            command.argument = rest;
        }

        rule->commands.push_back(std::move(command));
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
        case ScriptCommandKind::SetVariable: return "set";
        case ScriptCommandKind::AddVariable: return "add";
        case ScriptCommandKind::Move: return "move";
        case ScriptCommandKind::Rotate: return "rotate";
        case ScriptCommandKind::ApplyForce: return "apply_force";
        case ScriptCommandKind::SetVelocity: return "set_velocity";
        case ScriptCommandKind::Damage: return "damage";
        case ScriptCommandKind::Call: return "call";
        case ScriptCommandKind::Wait: return "wait";
        default: return "unknown";
    }
}

} // namespace aethera
