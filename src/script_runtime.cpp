#include "aethera/script_runtime.hpp"

namespace aethera {

namespace {
void set_animation_state(AnimationGraph& animation,const std::string& name){
    if(name=="idle")animation.set_state(AnimationState::Idle);else if(name=="walk")animation.set_state(AnimationState::Walk);else if(name=="run")animation.set_state(AnimationState::Run);else if(name=="jump")animation.set_state(AnimationState::Jump);else if(name=="fall")animation.set_state(AnimationState::Fall);else if(name=="land")animation.set_state(AnimationState::Land);else animation.set_state(AnimationState::Custom);
}
}

bool ScriptRuntime::run_trigger(const std::string& trigger,AnimationGraph& animation,BehaviorSystem& behavior){
    bool executed=false;
    for(const auto& rule:program_.rules){
        if(rule.trigger!=trigger)continue;
        if(rule.condition.valid){bool matches=false;if(!evaluate_expression(rule.condition,variables_,matches)||!matches)continue;}
        for(const auto& command:rule.commands){
            switch(command.kind){
                case ScriptCommandKind::SetState:set_animation_state(animation,command.argument);executed=true;break;
                case ScriptCommandKind::Transition:executed=animation.request(command.argument)||executed;break;
                case ScriptCommandKind::EmitEvent:behavior.emit(BehaviorEvent::Custom,BehaviorContext{command.argument,{},command.argument});executed=true;break;
                case ScriptCommandKind::SetFlag:behavior.set_flag(command.argument,true);variables_[command.argument]=true;executed=true;break;
                case ScriptCommandKind::ClearFlag:behavior.set_flag(command.argument,false);variables_[command.argument]=false;executed=true;break;
                case ScriptCommandKind::SetVariable:variables_[command.argument]=command.script_value;if(const auto* b=std::get_if<bool>(&command.script_value))behavior.set_flag(command.argument,*b);executed=true;break;
                case ScriptCommandKind::AddVariable:{auto it=variables_.find(command.argument);float value=0.0f;if(it!=variables_.end())if(const auto* p=std::get_if<float>(&it->second))value=*p;variables_[command.argument]=value+command.value;executed=true;break;}
                case ScriptCommandKind::Call:behavior.emit(BehaviorEvent::Custom,BehaviorContext{command.argument,{},command.argument});executed=true;break;
                case ScriptCommandKind::Wait:executed=true;break;
            }
        }
    }
    return executed;
}

} // namespace aethera
