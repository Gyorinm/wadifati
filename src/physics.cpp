#include "aethera/physics.hpp"

#include <algorithm>

namespace aethera {

void PhysicsWorld::add_point(BodyPoint point) {
    points_.push_back(point);
}

void PhysicsWorld::add_constraint(DistanceConstraint constraint) {
    constraints_.push_back(constraint);
}

void PhysicsWorld::step(float dt) {
    dt = clamp(dt, 0.0f, 1.0f / 30.0f);
    const float dt2 = dt * dt;

    for (auto& point : points_) {
        if (point.pinned) {
            point.previous_position = point.position;
            continue;
        }

        const Vec2 velocity = (point.position - point.previous_position) * damping;
        point.previous_position = point.position;
        point.position += velocity + gravity * (dt2 * point.inverse_mass);

        if (point.position.y > floor_y) {
            point.position.y = floor_y;
            point.previous_position.y = point.position.y;
        }
    }

    for (int iteration = 0; iteration < std::max(1, solver_iterations); ++iteration) {
        for (const auto& constraint : constraints_) {
            if (constraint.a >= points_.size() || constraint.b >= points_.size()) {
                continue;
            }

            auto& a = points_[constraint.a];
            auto& b = points_[constraint.b];
            const Vec2 delta = b.position - a.position;
            const float distance = length(delta);
            if (distance < 1e-6f) {
                continue;
            }

            const float inverse_mass_sum = a.inverse_mass + b.inverse_mass;
            if (inverse_mass_sum <= 0.0f) {
                continue;
            }

            const float error = (distance - constraint.rest_length) / distance;
            const Vec2 correction = delta * (constraint.stiffness * error);

            if (!a.pinned) {
                a.position += correction * (a.inverse_mass / inverse_mass_sum);
            }
            if (!b.pinned) {
                b.position -= correction * (b.inverse_mass / inverse_mass_sum);
            }
        }
    }
}

} // namespace aethera
