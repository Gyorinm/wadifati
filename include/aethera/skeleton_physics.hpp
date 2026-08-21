#pragma once

#include <cstddef>
#include <vector>

#include "aethera/physics.hpp"
#include "aethera/semantic.hpp"

namespace aethera {

class SkeletonPhysics {
public:
    bool build(const SemanticObject& semantic, const ImageAnalysisResult& analysis,
               PhysicsWorld& physics);

    void sync_to_semantic(const SemanticObject& semantic, const ImageAnalysisResult& analysis,
                          PhysicsWorld& physics, float position_scale = 1.0f);

    const std::vector<std::size_t>& semantic_to_physics() const {
        return semantic_to_physics_;
    }

private:
    std::vector<std::size_t> semantic_to_physics_;
};

} // namespace aethera
