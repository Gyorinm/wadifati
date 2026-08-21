#include <cassert>
#include <cstdio>
#include "aethera/image_pipeline.hpp"
#include "aethera/scene_file.hpp"
int main() {
    aethera::ImageRgba8 image;
    image.width=4; image.height=4; image.pixels.resize(4*4*4, 255);
    auto actor = aethera::ImagePipeline::build(image, 2, 2);
    const char* path = "aethera_scene_roundtrip.tmp";
    assert(aethera::SceneFile::save(path, actor));
    aethera::ImageActor loaded;
    assert(aethera::SceneFile::load(path, loaded));
    assert(loaded.image.valid());
    assert(loaded.mesh.size() == actor.mesh.size());
    assert(loaded.indices.size() == actor.indices.size());
    std::remove(path);
    return 0;
}
