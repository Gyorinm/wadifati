#pragma once

#include <cstddef>

#include "aethera/math.hpp"
#include "aethera/physics.hpp"

namespace aethera {

struct IKChain2D {
    std::size_t root{0};
    std::size_t joint{0};
    std::size_t end{0};
    float root_to_joint{0.0f};
    float joint_to_end{0.0f};
    float max_iterations{8.0f};
};

class IK2D {
public:
    static bool solve(IKChain2D chain, PhysicsWorld& physics, Vec2 target,
                      float tolerance = 1.0f);
};

} // namespace aethera
