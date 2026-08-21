#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "aethera/image.hpp"

namespace aethera {

enum class ImageObjectKind {
    Unknown,
    Region,
    Background
};

struct ImageRegion {
    Rect bounds{};
    std::size_t pixel_count{0};
    Vec2 centroid{};
    std::uint8_t average_alpha{0};
    ImageObjectKind kind{ImageObjectKind::Region};
};

struct ImageAnalysisOptions {
    std::uint8_t alpha_threshold{8};
    std::uint8_t background_tolerance{18};
    std::size_t min_region_pixels{32};
    bool detect_background{true};
    bool merge_small_regions{true};
};

struct ImageAnalysisResult {
    std::vector<ImageRegion> regions;
    int background_label{-1};
    std::vector<std::int32_t> labels;

    bool valid() const { return !regions.empty() && !labels.empty(); }
};

class ImageAnalyzer {
public:
    ImageAnalysisResult analyze(const ImageRgba8& image,
                                const ImageAnalysisOptions& options = {}) const;

    ImageObject create_object(const ImageRgba8& image,
                              const ImageAnalysisOptions& options = {}) const;
};

} // namespace aethera
