#pragma once

#include <cstdint>
#include <vector>

#include "aethera/image.hpp"
#include "aethera/vision_model.hpp"

namespace aethera {

// Extracts a mask into a tightly packed RGBA layer with transparent background.
ImageRgba8 extract_masked_layer(const ImageRgba8& image, const VisionMask& mask,
                                int& out_x, int& out_y);

} // namespace aethera
