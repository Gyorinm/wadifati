#include "aethera/script.hpp"
#include "aethera/script_expr.hpp"
#include "aethera/script_runtime.hpp"

#include <cassert>
#include <cmath>

int main(){
    aethera::ScriptExpression expression;
    assert(aethera::parse_expression("speed >= 2.5", expression));
    std::unordered_map<std::string,aethera::ScriptValue> vars;
    vars["speed"] = 3.0f;
    bool result=false;
    assert(aethera::evaluate_expression(expression,vars,result));
    assert(result);

    const char* source = R"(
object wolf
when update
    if energy > 10
    add energy -2
    set alert true
    call growl
)
";
    aethera::ScriptParser parser;
    aethera::ScriptProgram program;
    std::vector<aethera::ScriptError> errors;
    assert(parser.parse(source,program,errors));
    assert(program.rules.size()==1);

    aethera::AnimationGraph animation;
    aethera::BehaviorSystem behavior;
    aethera::ScriptRuntime runtime;
    runtime.load(program);
    runtime.set_variable("energy",20.0f);
    assert(runtime.run_trigger("update",animation,behavior));
    const auto it=runtime.variables().find("energy");
    assert(it!=runtime.variables().end());
    assert(std::fabs(std::get<float>(it->second)-18.0f)<1e-5f);
    assert(behavior.flag("alert"));
    return 0;
}
