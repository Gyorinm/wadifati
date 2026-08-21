#include "aethera/script_expr.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

namespace aethera {
namespace {
std::string trim(std::string s){
    std::size_t a=0,b=s.size();
    while(a<b&&std::isspace(static_cast<unsigned char>(s[a])))++a;
    while(b>a&&std::isspace(static_cast<unsigned char>(s[b-1])))--b;
    return s.substr(a,b-a);
}

bool parse_op(const std::string& s, CompareOp& op, std::size_t& pos) {
    static const char* ops[] = {"==","!=","<=",">=","<",">"};
    for(const char* candidate: ops){
        const std::string c(candidate);
        const auto p=s.find(c);
        if(p!=std::string::npos){
            op = c=="=="?CompareOp::Equal:c=="!="?CompareOp::NotEqual:c=="<"?CompareOp::Less:c=="<="?CompareOp::LessEqual:c==">"?CompareOp::Greater:CompareOp::GreaterEqual;
            pos=p; return true;
        }
    }
    return false;
}

bool parse_value(const std::string& raw, ScriptValue& out) {
    const std::string s=trim(raw);
    if(s=="true"){out=true;return true;}
    if(s=="false"){out=false;return true;}
    if(s.size()>=2 && ((s.front()=='\"'&&s.back()=='\"')||(s.front()=='\''&&s.back()=='\''))){out=s.substr(1,s.size()-2);return true;}
    try { out=std::stof(s); return true; } catch(...) {}
    out=s; return true;
}
}

bool parse_expression(const std::string& source, ScriptExpression& expression){
    expression={};
    std::size_t pos=0;
    if(!parse_op(source, expression.op, pos)) return false;
    expression.left=trim(source.substr(0,pos));
    if(expression.left.empty()) return false;
    if(!parse_value(source.substr(pos + (source.compare(pos,2,"==")==0 || source.compare(pos,2,"!=")==0 || source.compare(pos,2,"<=")==0 || source.compare(pos,2,">=")==0 ? 2 : 1)), expression.right)) return false;
    expression.valid=true; return true;
}

bool evaluate_expression(const ScriptExpression& expression,
                         const std::unordered_map<std::string, ScriptValue>& variables,
                         bool& result){
    result=false;
    if(!expression.valid) return false;
    const auto it=variables.find(expression.left);
    if(it==variables.end()) return false;
    const ScriptValue& a=it->second;
    const ScriptValue& b=expression.right;
    if(a.index()!=b.index()) return false;
    auto apply_num=[&](float x,float y){
        switch(expression.op){case CompareOp::Equal:result=x==y;break;case CompareOp::NotEqual:result=x!=y;break;case CompareOp::Less:result=x<y;break;case CompareOp::LessEqual:result=x<=y;break;case CompareOp::Greater:result=x>y;break;case CompareOp::GreaterEqual:result=x>=y;break;}
    };
    if(const auto* x=std::get_if<float>(&a)){ apply_num(*x,std::get<float>(b)); return true; }
    if(const auto* x=std::get_if<bool>(&a)){
        const bool y=std::get<bool>(b);
        result=expression.op==CompareOp::Equal?*x==y:expression.op==CompareOp::NotEqual?*x!=y:false;
        return expression.op==CompareOp::Equal || expression.op==CompareOp::NotEqual;
    }
    const auto& x=std::get<std::string>(a); const auto& y=std::get<std::string>(b);
    switch(expression.op){case CompareOp::Equal:result=x==y;break;case CompareOp::NotEqual:result=x!=y;break;case CompareOp::Less:result=x<y;break;case CompareOp::LessEqual:result=x<=y;break;case CompareOp::Greater:result=x>y;break;case CompareOp::GreaterEqual:result=x>=y;break;}
    return true;
}

} // namespace aethera
