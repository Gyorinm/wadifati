#include <cassert>
#include <cmath>
#include <vector>
#include "aethera/bone_binding.hpp"
using namespace aethera;
int main() {
    std::vector<VisionKeypoint> points(2);
    points[0].normalized_position = {0.25f, 0.5f};
    points[1].normalized_position = {0.75f, 0.5f};
    auto bones = build_bones_from_keypoints(points, 100, 80);
    assert(bones.size() == 2);
    assert(std::abs(bones[0].bind_position.x - 25.0f) < 1e-5f);
    assert(std::abs(bones[1].bind_position.x - 75.0f) < 1e-5f);
    std::vector<MeshVertex> vertices(1);
    vertices[0].position = {50.0f, 40.0f};
    assign_layer_bone_weights(vertices, bones, points, 100, 80);
    assert(std::abs(vertices[0].weight0 + vertices[0].weight1 - 1.0f) < 1e-5f);
    return 0;
}
