#include "aethera/ik.hpp"

#include <cassert>
#include <cmath>

int main() {
    aethera::PhysicsWorld physics;
    physics.add_point({{100.0f, 100.0f}, {100.0f, 100.0f}, 0.0f, true});
    physics.add_point({{140.0f, 100.0f}, {140.0f, 100.0f}, 1.0f, false});
    physics.add_point({{180.0f, 100.0f}, {180.0f, 100.0f}, 1.0f, false});

    const bool solved = aethera::IK2D::solve({0, 1, 2, 40.0f, 40.0f, 8.0f}, physics,
                                               {140.0f, 140.0f}, 1.0f);
    assert(solved);
    assert(aethera::length(physics.points()[2].position - aethera::Vec2{140.0f, 140.0f}) <= 2.0f);
    return 0;
}
