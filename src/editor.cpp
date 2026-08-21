#include "aethera/editor.hpp"
#include "aethera/scene_file.hpp"
#include <algorithm>
namespace aethera {
void Editor::set_actor(ImageActor actor){ actor_=std::move(actor); has_actor_=true; selected_=0; }
bool Editor::save(const std::string& path) const { return has_actor_ && SceneFile::save(path, actor_); }
bool Editor::load(const std::string& path){ ImageActor next; if(!SceneFile::load(path,next)) return false; set_actor(std::move(next)); return true; }
void Editor::select_joint(std::size_t index){ if(has_actor_ && index<actor_.skeleton.size()) selected_=index; }
void Editor::move_selected(Vec2 delta){ if(!has_actor_ || selected_>=actor_.skeleton.size()) return; actor_.skeleton[selected_].current = actor_.skeleton[selected_].current + delta; ImagePipeline::update_bones(actor_); }
void Editor::rotate_selected(float radians){ if(!has_actor_ || selected_>=actor_.skeleton.size()) return; actor_.skeleton[selected_].current_rotation += radians; ImagePipeline::update_bones(actor_); }
}
