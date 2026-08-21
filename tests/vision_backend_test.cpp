#include <cassert>
#include "aethera/image_loader.hpp"
#include "aethera/vision_backend.hpp"
using namespace aethera;
int main(){
    ImageRgba8 image; image.width=8; image.height=8; image.pixels.resize(8*8*4,255);
    HeuristicVisionBackend backend; VisionResultLite result;
    assert(backend.analyze(image,result));
    assert(result.mask_width==8 && result.mask_height==8);
    assert(!result.masks.empty());
    assert(result.joints.size()==2);
    return 0;
}
