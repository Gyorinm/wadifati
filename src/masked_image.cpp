#include "aethera/masked_image.hpp"

#include <algorithm>

namespace aethera {

ImageRgba8 extract_masked_layer(const ImageRgba8& image, const VisionMask& mask,
                                int& out_x, int& out_y) {
    out_x = 0;
    out_y = 0;
    if (image.width <= 0 || image.height <= 0 || mask.width <= 0 || mask.height <= 0 ||
        mask.pixels.size() != static_cast<std::size_t>(mask.width * mask.height)) {
        return {};
    }

    const int width = std::min(image.width, mask.width);
    const int height = std::min(image.height, mask.height);
    int min_x = width, min_y = height, max_x = -1, max_y = -1;
    const float threshold = std::clamp(mask.threshold, 0.0f, 1.0f);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float value = mask.pixels[static_cast<std::size_t>(y * mask.width + x)] / 255.0f;
            if (value >= threshold) {
                min_x = std::min(min_x, x);
                min_y = std::min(min_y, y);
                max_x = std::max(max_x, x);
                max_y = std::max(max_y, y);
            }
        }
    }

    if (max_x < min_x || max_y < min_y) return {};

    ImageRgba8 layer;
    layer.width = max_x - min_x + 1;
    layer.height = max_y - min_y + 1;
    layer.pixels.assign(static_cast<std::size_t>(layer.width * layer.height * 4), 0);
    out_x = min_x;
    out_y = min_y;

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            const std::size_t source = static_cast<std::size_t>((y * image.width + x) * 4);
            const std::size_t mask_index = static_cast<std::size_t>(y * mask.width + x);
            const std::size_t target = static_cast<std::size_t>(((y - min_y) * layer.width + (x - min_x)) * 4);
            const float alpha = mask.pixels[mask_index] / 255.0f;
            layer.pixels[target + 0] = image.pixels[source + 0];
            layer.pixels[target + 1] = image.pixels[source + 1];
            layer.pixels[target + 2] = image.pixels[source + 2];
            layer.pixels[target + 3] = static_cast<std::uint8_t>(image.pixels[source + 3] * alpha);
        }
    }
    return layer;
}

} // namespace aethera
