#include "aethera/script_scene.hpp"

namespace aethera {

ImageNode* ScriptSceneControl::node(const std::string& name) {
    if (object_ == nullptr) return nullptr;
    for (auto& item : object_->nodes()) {
        if (item.name == name) return &item;
    }
    return nullptr;
}

bool ScriptSceneControl::rotate(const std::string& name, float radians) {
    auto* item = node(name);
    if (!item) return false;
    item->local.rotation += radians;
    return true;
}

bool ScriptSceneControl::move(const std::string& name, Vec2 delta) {
    auto* item = node(name);
    if (!item) return false;
    item->local.position += delta;
    return true;
}

bool ScriptSceneControl::scale(const std::string& name, Vec2 factor) {
    auto* item = node(name);
    if (!item) return false;
    item->local.scale.x *= factor.x;
    item->local.scale.y *= factor.y;
    return true;
}

bool ScriptSceneControl::visible(const std::string& name, bool value) {
    auto* item = node(name);
    if (!item) return false;
    item->visible = value;
    return true;
}

void ScriptSceneControl::sync() {
    if (object_ != nullptr) object_->update_world_transforms();
}

} // namespace aethera
