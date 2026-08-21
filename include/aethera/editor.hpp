#pragma once
#include <cstddef>
#include <string>
#include "aethera/image_pipeline.hpp"
namespace aethera {
class Editor {
public:
    void set_actor(ImageActor actor);
    ImageActor* actor() { return has_actor_ ? &actor_ : nullptr; }
    const ImageActor* actor() const { return has_actor_ ? &actor_ : nullptr; }
    bool save(const std::string& path) const;
    bool load(const std::string& path);
    void select_joint(std::size_t index);
    void move_selected(Vec2 delta);
    void rotate_selected(float radians);
    std::size_t selected_joint() const { return selected_; }
private:
    ImageActor actor_{};
    bool has_actor_{false};
    std::size_t selected_{0};
};
}
