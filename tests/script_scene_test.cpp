#include "aethera/image.hpp"
#include "aethera/image_object.hpp"
#include "aethera/script_scene.hpp"

#include <cassert>
#include <cmath>

int main() {
    aethera::ImageRgba8 image;
    image.width = 8;
    image.height = 8;
    image.pixels.assign(8 * 8 * 4, 255);

    aethera::ImageObject object("scene");
    object.set_image(&image);

    aethera::ImageNode arm;
    arm.name = "arm";
    arm.local.position = {10.0f, 5.0f};
    object.add_node(arm);
    object.update_world_transforms();

    aethera::ScriptSceneControl control(&object);
    assert(control.rotate("arm", 0.5f));
    assert(control.move("arm", {4.0f, -2.0f}));
    assert(control.scale("arm", {2.0f, 0.5f}));
    assert(control.visible("arm", false));
    control.sync();

    const auto* node = object.node(0);
    assert(node != nullptr);
    assert(std::fabs(node->local.rotation - 0.5f) < 1e-5f);
    assert(std::fabs(node->local.position.x - 14.0f) < 1e-5f);
    assert(std::fabs(node->local.position.y - 3.0f) < 1e-5f);
    assert(std::fabs(node->local.scale.x - 2.0f) < 1e-5f);
    assert(std::fabs(node->local.scale.y - 0.5f) < 1e-5f);
    assert(!node->visible);
    return 0;
}
