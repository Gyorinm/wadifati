#include <iostream>
#include "aethera/image_loader.hpp"
#include "aethera/image_pipeline.hpp"
#include "aethera/scene_file.hpp"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: aethera_import <image> <scene>\n";
        return 2;
    }
    aethera::ImageRgba8 image;
    if (!aethera::ImageLoader::load(argv[1], image)) {
        std::cerr << "failed to load image: " << argv[1] << '\n';
        return 1;
    }
    auto actor = aethera::ImagePipeline::build(image);
    if (!aethera::SceneFile::save(argv[2], actor)) {
        std::cerr << "failed to save scene: " << argv[2] << '\n';
        return 1;
    }
    std::cout << "Aethera scene created: " << argv[2] << '\n';
    return 0;
}
