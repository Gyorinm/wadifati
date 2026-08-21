#pragma once
#include <string>
#include "aethera/image.hpp"
#include "aethera/mesh.hpp"
namespace aethera {
struct VisionJoint {
    std::string name;
    int parent{-1};
    Vec2 normalized{};
    float confidence{0.0f};
};
struct VisionResultLite {
    std::vector<VisionJoint> joints;
    std::vector<std::vector<std::uint8_t>> masks;
    int mask_width{0};
    int mask_height{0};
};
class VisionBackend {
public:
    virtual ~VisionBackend() = default;
    virtual bool analyze(const ImageRgba8& image, VisionResultLite& result) = 0;
};
class HeuristicVisionBackend final : public VisionBackend {
public:
    bool analyze(const ImageRgba8& image, VisionResultLite& result) override;
};
}
