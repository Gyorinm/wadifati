#pragma once

#include <cstddef>
#include <vector>

#include "aethera/math.hpp"

namespace aethera {

struct BodyPoint {
    Vec2 position{};
    Vec2 previous_position{};
    float inverse_mass{1.0f};
    bool pinned{false};
};

struct DistanceConstraint {
    std::size_t a{0};
    std::size_t b{0};
    float rest_length{0.0f};
    float stiffness{1.0f};
};

class PhysicsWorld {
public:
    Vec2 gravity{0.0f, 980.0f};
    float damping{0.995f};
    float floor_y{540.0f};
    int solver_iterations{8};

    void add_point(BodyPoint point);
    void add_constraint(DistanceConstraint constraint);
    void step(float dt);

    std::vector<BodyPoint>& points() { return points_; }
    const std::vector<BodyPoint>& points() const { return points_; }
    const std::vector<DistanceConstraint>& constraints() const { return constraints_; }

private:
    std::vector<BodyPoint> points_;
    std::vector<DistanceConstraint> constraints_;
};

} // namespace aethera
