#include "aethera/image_analyzer.hpp"

#include <cassert>
#include <cstdint>

int main() {
    aethera::ImageRgba8 image;
    image.width = 10;
    image.height = 10;
    image.pixels.assign(10u * 10u * 4u, 0u);

    // Opaque background-like border.
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            const std::size_t p = (static_cast<std::size_t>(y) * 10u + static_cast<std::size_t>(x)) * 4u;
            image.pixels[p + 0] = 20;
            image.pixels[p + 1] = 20;
            image.pixels[p + 2] = 20;
            image.pixels[p + 3] = 255;
        }
    }

    // Two separated foreground regions.
    for (int y = 2; y < 5; ++y) {
        for (int x = 2; x < 5; ++x) {
            const std::size_t p = (static_cast<std::size_t>(y) * 10u + static_cast<std::size_t>(x)) * 4u;
            image.pixels[p + 0] = 220;
            image.pixels[p + 1] = 40;
            image.pixels[p + 2] = 40;
        }
    }
    for (int y = 6; y < 9; ++y) {
        for (int x = 6; x < 9; ++x) {
            const std::size_t p = (static_cast<std::size_t>(y) * 10u + static_cast<std::size_t>(x)) * 4u;
            image.pixels[p + 0] = 40;
            image.pixels[p + 1] = 180;
            image.pixels[p + 2] = 60;
        }
    }

    aethera::ImageAnalyzer analyzer;
    aethera::ImageAnalysisOptions options;
    options.min_region_pixels = 4;
    const auto result = analyzer.analyze(image, options);

    assert(result.valid());
    assert(result.regions.size() == 2);
    assert(result.labels.size() == 100);

    const auto object = analyzer.create_object(image, options);
    assert(object.nodes().size() == 2);
    assert(object.image() == &image);
    assert(object.world_transform(0).position.x > 0.0f);

    return 0;
}
