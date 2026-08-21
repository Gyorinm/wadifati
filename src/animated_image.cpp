#include "aethera/animated_image.hpp"

namespace aethera {

void AnimatedImage::set_bone_position(std::size_t part, std::size_t bone, Vec2 position) {
    if (part >= result_.parts.size() || bone >= result_.parts[part].bones.size()) return;
    result_.parts[part].bones[bone].current_position = position;
}

void AnimatedImage::set_bone_rotation(std::size_t part, std::size_t bone, float radians) {
    if (part >= result_.parts.size() || bone >= result_.parts[part].bones.size()) return;
    result_.parts[part].bones[bone].current_rotation = radians;
}

void AnimatedImage::reset_pose() {
    for (auto& part : result_.parts) {
        for (auto& bone : part.bones) {
            bone.current_position = bone.bind_position;
            bone.current_rotation = bone.bind_rotation;
        }
    }
}

} // namespace aethera
