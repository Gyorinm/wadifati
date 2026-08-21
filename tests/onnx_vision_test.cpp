#include "aethera/onnx_vision.hpp"

#include <cassert>

int main() {
    aethera::OnnxVisionModelConfig config;
    config.model_path = "missing-model.onnx";
    aethera::OnnxVisionBackend backend(config);

#ifndef AETHERA_HAS_ONNX_RUNTIME
    assert(!backend.available());
#endif

    aethera::ImageRgba8 image;
    image.width = 2;
    image.height = 2;
    image.pixels.assign(16, 255);

    const auto result = backend.analyze(image);
    assert(!result.diagnostics.empty());
    return 0;
}
