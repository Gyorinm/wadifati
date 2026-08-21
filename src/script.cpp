#include "aethera/script.hpp"

#include <cctype>
#include <sstream>

namespace aethera {
namespace {
std::string trim(std::string value){std::size_t a=0,b=value.size();while(a<b&&std::isspace(static_cast<unsigned char>(value[a])))++a;while(b>a&&std::isspace(static_cast<unsigned char>(value[b-1])))--b;return value.substr(a,b-a);}
bool starts_with(const std::string& v,const std::string& p){return v.rfind(p,0)==0;}
void err(std::vector<ScriptError>& e,std::size_t l,const std::string& m){e.push_back({l,m});}
}

bool ScriptParser::parse(const std::string& source, ScriptProgram& program,std::vector<ScriptError>& errors) const{
    program={}; errors.clear(); ScriptRule* rule=nullptr;
    std::istringstream input(source); std::string raw; std::size_t line=0;
    while(std::getline(input,raw)){
        ++line; const std::string s=trim(raw); if(s.empty()||s[0]=='#') continue;
        if(starts_with(s,"object ")){program.object_name=trim(s.substr(7));rule=nullptr;if(program.object_name.empty())err(errors,line,"object name is required");continue;}
        if(starts_with(s,"when ")){const std::string t=trim(s.substr(5));if(t.empty()){err(errors,line,"event name is required");rule=nullptr;}else{program.rules.push_back({t,{}, {}});rule=&program.rules.back();}continue;}
        if(starts_with(s,"if ")){if(!rule)err(errors,line,"condition must follow a when rule");else if(!parse_expression(trim(s.substr(3)),rule->condition))err(errors,line,"invalid condition");continue;}
        if(!rule){err(errors,line,"command must belong to a when rule");continue;}
        ScriptCommand c; std::string rest;
        if(starts_with(s,"set_state ")){c.kind=ScriptCommandKind::SetState;rest=trim(s.substr(10));}
        else if(starts_with(s,"transition ")){c.kind=ScriptCommandKind::Transition;rest=trim(s.substr(11));}
        else if(starts_with(s,"emit ")){c.kind=ScriptCommandKind::EmitEvent;rest=trim(s.substr(5));}
        else if(starts_with(s,"set_flag ")){c.kind=ScriptCommandKind::SetFlag;rest=trim(s.substr(9));}
        else if(starts_with(s,"clear_flag ")){c.kind=ScriptCommandKind::ClearFlag;rest=trim(s.substr(11));}
        else if(starts_with(s,"set ")){c.kind=ScriptCommandKind::SetVariable;std::istringstream ss(s.substr(4));ss>>c.argument;std::string rawv;std::getline(ss,rawv);rawv=trim(rawv);if(c.argument.empty()||rawv.empty()){err(errors,line,"set expects a variable and value");continue;}if(!parse_value(rawv,c.script_value)){err(errors,line,"invalid value");continue;}}
        else if(starts_with(s,"add ")){c.kind=ScriptCommandKind::AddVariable;std::istringstream ss(s.substr(4));ss>>c.argument>>c.value;if(c.argument.empty()||ss.fail()){err(errors,line,"add expects variable and numeric value");continue;}}
        else if(starts_with(s,"call ")){c.kind=ScriptCommandKind::Call;rest=trim(s.substr(5));}
        else if(starts_with(s,"wait ")){c.kind=ScriptCommandKind::Wait;try{c.value=std::stof(trim(s.substr(5)));}catch(...){err(errors,line,"wait expects a numeric duration");continue;}if(c.value<0){err(errors,line,"wait duration cannot be negative");continue;}}
        else{err(errors,line,"unknown command: "+s);continue;}
        if((c.kind!=ScriptCommandKind::Wait&&c.kind!=ScriptCommandKind::SetVariable&&c.kind!=ScriptCommandKind::AddVariable)&&rest.empty()){err(errors,line,"command argument is required");continue;}
        if(!rest.empty())c.argument=rest;
        rule->commands.push_back(std::move(c));
    }
    if(program.object_name.empty())err(errors,0,"script must declare an object");
    return errors.empty();
}

const char* script_command_name(ScriptCommandKind kind){switch(kind){case ScriptCommandKind::SetState:return "set_state";case ScriptCommandKind::Transition:return "transition";case ScriptCommandKind::EmitEvent:return "emit";case ScriptCommandKind::SetFlag:return "set_flag";case ScriptCommandKind::ClearFlag:return "clear_flag";case ScriptCommandKind::SetVariable:return "set";case ScriptCommandKind::AddVariable:return "add";case ScriptCommandKind::Call:return "call";case ScriptCommandKind::Wait:return "wait";default:return "unknown";}}

} // namespace aethera
