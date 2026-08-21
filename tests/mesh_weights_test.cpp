#include <cassert>
#include <cmath>
#include <vector>

#include "aethera/mesh_weights.hpp"

using namespace aethera;

int main() {
    std::vector<MeshVertex> vertices(1);
    vertices[0].position = {2.0f, 0.0f};
    std::vector<MeshBone> bones(2);
    bones[0].bind_position = {0.0f, 0.0f};
    bones[1].bind_position = {4.0f, 0.0f};
    std::vector<VisionKeypoint> points(2);
    points[0].normalized_position = {0.0f, 0.0f};
    points[1].normalized_position = {4.0f, 0.0f};
    points[0].confidence = points[1].confidence = 1.0f;
    assign_nearest_bone_weights(vertices, bones, points);
    assert(vertices[0].bone0 == 0 || vertices[0].bone0 == 1);
    assert(vertices[0].weight0 >= 0.0f && vertices[0].weight0 <= 1.0f);
    assert(vertices[0].weight1 >= 0.0f && vertices[0].weight1 <= 1.0f);
    assert(std::abs(vertices[0].weight0 + vertices[0].weight1 - 1.0f) < 1e-5f);
    return 0;
}
