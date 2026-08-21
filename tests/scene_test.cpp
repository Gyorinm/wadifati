#include "aethera/scene.hpp"

#include <cassert>
#include <cstdio>

int main() {
    aethera::ImageRgba8 image;
    image.width = 4;
    image.height = 4;
    image.pixels.assign(4 * 4 * 4, 255);

    aethera::ImageObject object("test");
    object.set_image(&image);

    aethera::ImageNode root;
    root.name = "root";
    root.local.position = {10.0f, 20.0f};
    const auto root_id = object.add_node(root);

    aethera::ImageNode child;
    child.name = "arm";
    child.parent = root_id;
    child.local.rotation = 0.5f;
    child.visual.source = {1, 2, 3, 4};
    object.add_node(child);
    object.update_world_transforms();

    const aethera::Scene original = aethera::capture_scene(object, "test_scene", "hero.png");
    assert(original.name == "test_scene");
    assert(original.image_asset == "hero.png");
    assert(original.nodes.size() == 2);

    const char* path = "aethera_scene_test.scene";
    aethera::SceneSerializer serializer;
    std::string error;
    assert(serializer.save(original, path, &error));

    aethera::Scene loaded;
    assert(serializer.load(path, loaded, &error));
    assert(loaded.name == original.name);
    assert(loaded.image_asset == original.image_asset);
    assert(loaded.nodes.size() == original.nodes.size());
    assert(loaded.nodes[1].visual.source.width == 3.0f);
    assert(loaded.nodes[1].transform.rotation == 0.5f);

    object.nodes()[1].local.rotation = 0.0f;
    object.nodes()[0].local.position = {};
    assert(aethera::apply_scene(loaded, object));
    assert(object.nodes()[0].local.position.x == 10.0f);
    assert(object.nodes()[1].local.rotation == 0.5f);

    std::remove(path);
    return 0;
}
