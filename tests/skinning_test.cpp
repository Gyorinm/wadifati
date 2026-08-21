#include "aethera/skinning.hpp"

#include <cassert>

int main() {
    using namespace aethera;

    ImageRgba8 image;
    image.width = 64;
    image.height = 64;
    image.pixels.resize(64 * 64 * 4, 255);

    ImageAnalysisResult analysis;
    analysis.labels = {0, 1};
    analysis.regions.resize(2);
    analysis.regions[0].centroid = {10.0f, 10.0f};
    analysis.regions[0].bounds = {0.0f, 0.0f, 20.0f, 20.0f};
    analysis.regions[1].centroid = {30.0f, 10.0f};
    analysis.regions[1].bounds = {20.0f, 0.0f, 20.0f, 20.0f};

    SemanticObject semantic;
    semantic.regions = {0, 1};
    semantic.joints = {
        {"root", analysis.regions[0].centroid, 0},
        {"child", analysis.regions[1].centroid, 1}
    };
    semantic.links = {{0, 1, 20.0f}};

    ImageObject object("test");
    object.set_image(&image);
    ImageNode node;
    node.name = "part_1";
    node.visual.source = analysis.regions[1].bounds;
    object.add_node(node);

    ImageSkinner skinner;
    assert(skinner.build(semantic, analysis, object));
    assert(skinner.bindings().size() == 1);

    PhysicsWorld physics;
    physics.add_point({analysis.regions[0].centroid, analysis.regions[0].centroid, 0.0f, true});
    physics.add_point({analysis.regions[1].centroid, analysis.regions[1].centroid, 1.0f, false});
    physics.points()[1].position = {50.0f, 20.0f};

    skinner.apply_pose(physics, object);

    const ImageNode& deformed = object.nodes()[0];
    assert(deformed.local.position.x == 50.0f);
    assert(deformed.local.position.y == 20.0f);
    assert(deformed.local.scale.x > 1.0f);

    return 0;
}
