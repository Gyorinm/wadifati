#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace aethera {

enum class CompareOp { Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual };

using ScriptValue = std::variant<float, bool, std::string>;

struct ScriptExpression {
    std::string left;
    CompareOp op{CompareOp::Equal};
    ScriptValue right{false};
    bool valid{false};
};

bool evaluate_expression(const ScriptExpression& expression,
                         const std::unordered_map<std::string, ScriptValue>& variables,
                         bool& result);

bool parse_expression(const std::string& source, ScriptExpression& expression);

} // namespace aethera
