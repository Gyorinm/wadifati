#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "aethera/math.hpp"

namespace aethera {

struct ImageRgba8 {
    int width{0};
    int height{0};
    std::vector<std::uint8_t> pixels;

    bool valid() const;
    const std::uint8_t* data() const { return pixels.data(); }
    std::uint8_t* data() { return pixels.data(); }
};

struct ImagePart {
    std::string name;
    Rect source{};
    Vec2 pivot{0.5f, 0.5f};
    Vec2 position{};
    float rotation{0.0f};
    Vec2 scale{1.0f, 1.0f};
    float opacity{1.0f};
};

class ImageAsset {
public:
    ImageAsset() = default;
    explicit ImageAsset(std::string source_path) : source_path_(std::move(source_path)) {}

    bool load_ppm(const std::string& path);
    bool valid() const { return image_.valid(); }
    const ImageRgba8& image() const { return image_; }
    const std::string& source_path() const { return source_path_; }

private:
    std::string source_path_;
    ImageRgba8 image_;
};

} // namespace aethera
