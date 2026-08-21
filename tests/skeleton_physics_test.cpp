#include "aethera/skeleton_physics.hpp"

#include <cassert>

using namespace aethera;

int main() {
    ImageAnalysisResult analysis;
    analysis.labels.resize(400, 0);
    analysis.regions.push_back({Rect{10, 10, 20, 20}, 100, Vec2{20, 20}, 255, ImageObjectKind::Region});
    analysis.regions.push_back({Rect{10, 50, 20, 20}, 100, Vec2{20, 60}, 255, ImageObjectKind::Region});

    SemanticObject semantic;
    semantic.kind = SemanticKind::Human;
    semantic.joints.push_back({"root", analysis.regions[0].centroid, 0});
    semantic.joints.push_back({"child", analysis.regions[1].centroid, 1});
    semantic.links.push_back({0, 1, 40.0f});

    PhysicsWorld physics;
    SkeletonPhysics bridge;
    assert(bridge.build(semantic, analysis, physics));
    assert(physics.points().size() == 2);
    assert(physics.constraints().size() == 1);
    assert(physics.points()[0].pinned);

    const Vec2 before = physics.points()[1].position;
    physics.step(1.0f / 60.0f);
    const Vec2 after = physics.points()[1].position;
    assert(after.y != before.y);

    return 0;
}
