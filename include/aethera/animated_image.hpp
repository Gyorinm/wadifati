#pragma once

#include <vector>
#include "aethera/image_reconstruction.hpp"

namespace aethera {

class AnimatedImage {
public:
    explicit AnimatedImage(ImageReconstructionResult result) : result_(std::move(result)) {}

    ImageReconstructionResult& reconstruction() { return result_; }
    const ImageReconstructionResult& reconstruction() const { return result_; }

    void set_bone_position(std::size_t part, std::size_t bone, Vec2 position);
    void set_bone_rotation(std::size_t part, std::size_t bone, float radians);
    void reset_pose();

private:
    ImageReconstructionResult result_;
};

} // namespace aethera
