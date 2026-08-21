#pragma once

#include <cstddef>
#include <vector>

#include "aethera/image_object.hpp"
#include "aethera/physics.hpp"
#include "aethera/semantic.hpp"

namespace aethera {

struct SkinBinding {
    std::size_t node{static_cast<std::size_t>(-1)};
    std::size_t joint_a{static_cast<std::size_t>(-1)};
    std::size_t joint_b{static_cast<std::size_t>(-1)};
    Vec2 bind_a{};
    Vec2 bind_b{};
    float width{1.0f};
};

class ImageSkinner {
public:
    bool build(const SemanticObject& semantic,
               const ImageAnalysisResult& analysis,
               ImageObject& object);

    void apply_pose(const PhysicsWorld& physics,
                    ImageObject& object) const;

    const std::vector<SkinBinding>& bindings() const { return bindings_; }

private:
    std::vector<SkinBinding> bindings_;
};

} // namespace aethera
