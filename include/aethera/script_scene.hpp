#pragma once

#include <cstddef>
#include <string>

#include "aethera/image_object.hpp"

namespace aethera {

class ScriptSceneControl {
public:
    explicit ScriptSceneControl(ImageObject* object) : object_(object) {}

    ImageNode* node(const std::string& name);
    bool rotate(const std::string& name, float radians);
    bool move(const std::string& name, Vec2 delta);
    bool scale(const std::string& name, Vec2 factor);
    bool visible(const std::string& name, bool value);
    void sync();

private:
    ImageObject* object_{};
};

} // namespace aethera
