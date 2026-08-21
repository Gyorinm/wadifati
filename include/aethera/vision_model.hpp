#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "aethera/image.hpp"
#include "aethera/semantic.hpp"

namespace aethera {

struct VisionMask {
    int width{0};
    int height{0};
    std::vector<std::uint8_t> pixels;
    float threshold{0.5f};
};

struct VisionKeypoint {
    std::string name;
    Vec2 normalized_position{};
    float confidence{0.0f};
};

struct VisionModelOutput {
    std::vector<float> semantic_scores;
    std::vector<VisionMask> masks;
    std::vector<VisionKeypoint> keypoints;
};

struct VisionModelContract {
    std::size_t input_width{256};
    std::size_t input_height{256};
    std::size_t semantic_classes{0};
    std::size_t keypoint_count{0};
};

const char* vision_semantic_class_name(std::size_t index);
SemanticKind vision_semantic_kind(std::size_t index);

VisionResult merge_model_output(const ImageRgba8& image,
                                const VisionModelOutput& output,
                                const VisionOptions& options = {});

} // namespace aethera
