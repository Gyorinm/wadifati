#include "aethera/vision.hpp"

#include <cassert>

int main() {
    aethera::ImageRgba8 image;
    image.width = 32;
    image.height = 16;
    image.pixels.assign(static_cast<std::size_t>(image.width * image.height * 4), 0);

    for (int y = 2; y < 14; ++y) {
        for (int x = 5; x < 12; ++x) {
            const std::size_t i = static_cast<std::size_t>((y * image.width + x) * 4);
            image.pixels[i + 0] = 255;
            image.pixels[i + 3] = 255;
        }
    }
    for (int y = 4; y < 12; ++y) {
        for (int x = 18; x < 25; ++x) {
            const std::size_t i = static_cast<std::size_t>((y * image.width + x) * 4);
            image.pixels[i + 1] = 255;
            image.pixels[i + 3] = 255;
        }
    }

    aethera::VisionSystem vision;
    const auto result = vision.analyze(image);
    assert(result.valid());
    assert(vision.backend().backend() == aethera::VisionBackend::Heuristic);
    assert(!result.diagnostics.empty());
    return 0;
}
