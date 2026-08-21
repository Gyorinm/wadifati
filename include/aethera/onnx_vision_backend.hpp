#pragma once
#include <memory>
#include <string>
#include "aethera/vision_backend.hpp"
namespace aethera {
class OnnxVisionBackend final : public VisionBackend {
public:
    explicit OnnxVisionBackend(const std::string& model_path);
    ~OnnxVisionBackend() override;
    bool available() const;
    bool analyze(const ImageRgba8& image, VisionResultLite& result) override;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
