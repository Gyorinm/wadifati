#include "aethera/vision_backend.hpp"
#include <algorithm>
namespace aethera {
bool HeuristicVisionBackend::analyze(const ImageRgba8& image, VisionResultLite& result) {
    result = {};
    if (!image.valid()) return false;
    result.mask_width = image.width; result.mask_height = image.height;
    result.masks.emplace_back(image.pixels.size() / 4, 0);
    auto& mask = result.masks.back();
    for (std::size_t p=0,i=0; p<image.pixels.size(); p+=4,++i) mask[i]=image.pixels[p+3];
    VisionJoint root; root.name="root"; root.parent=-1; root.normalized={0.5f,0.5f}; root.confidence=1.0f;
    VisionJoint tip; tip.name="tip"; tip.parent=0; tip.normalized={0.75f,0.5f}; tip.confidence=0.5f;
    result.joints={root,tip};
    return true;
}
}
