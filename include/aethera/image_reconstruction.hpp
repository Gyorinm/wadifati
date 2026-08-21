#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "aethera/image_object.hpp"
#include "aethera/vision_model.hpp"

namespace aethera {

struct ReconstructedPart {
    std::string name;
    std::size_t mask_index{static_cast<std::size_t>(-1)};
    std::size_t anchor_keypoint{static_cast<std::size_t>(-1)};
    Rect bounds{};
    Vec2 pivot{0.5f, 0.5f};
    Vec2 position{};
    float confidence{0.0f};
};

struct ImageReconstructionResult {
    ImageObject object;
    std::vector<ReconstructedPart> parts;
    std::vector<std::string> diagnostics;

    explicit ImageReconstructionResult(std::string name = {}) : object(std::move(name)) {}
};

class ImageReconstructor {
public:
    ImageReconstructionResult reconstruct(const ImageRgba8& image,
                                          const VisionModelOutput& output,
                                          const VisionResult& semantic = {}) const;
};

} // namespace aethera
