#pragma once

#include <cstddef>
#include <string>

#include "aethera/vision.hpp"

namespace aethera {

struct OnnxVisionModelConfig {
    std::string model_path;
    std::size_t input_width{256};
    std::size_t input_height{256};
    std::size_t classes{0};
    float mask_threshold{0.5f};
};

// Optional ONNX Runtime adapter. The backend is compiled only when the
// dependency is available. Its model contract is deliberately small:
// input:  [1,3,H,W] float32 RGB in [0,1]
// output0: [1,C] semantic logits (optional)
// output1: [1,1,H,W] foreground probability (optional)
// output2: [1,K,3] normalized keypoints x,y,confidence (optional)
class OnnxVisionBackend final : public VisionBackendInterface {
public:
    explicit OnnxVisionBackend(OnnxVisionModelConfig config);
    ~OnnxVisionBackend() override;

    VisionBackend backend() const override { return VisionBackend::ExternalModel; }
    VisionResult analyze(const ImageRgba8& image,
                         const VisionOptions& options = {}) const override;

    bool available() const;
    const std::string& error() const { return error_; }

private:
    OnnxVisionModelConfig config_;
    std::string error_;
    void* session_{nullptr};
};

} // namespace aethera
