#include "aethera/onnx_vision_backend.hpp"
#ifdef AETHERA_HAS_ONNX_RUNTIME
#include <onnxruntime_cxx_api.h>
#endif
namespace aethera {
struct OnnxVisionBackend::Impl {
#ifdef AETHERA_HAS_ONNX_RUNTIME
    Ort::Env env{ORT_LOGGING_LEVEL_WARNING, "aethera"};
    Ort::SessionOptions options;
    std::unique_ptr<Ort::Session> session;
    explicit Impl(const std::string& path) {
        options.SetIntraOpNumThreads(1);
        options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
        session = std::make_unique<Ort::Session>(env, path.c_str(), options);
    }
#else
    bool available{false};
    explicit Impl(const std::string&) {}
#endif
};
OnnxVisionBackend::OnnxVisionBackend(const std::string& model_path): impl_(std::make_unique<Impl>(model_path)) {}
OnnxVisionBackend::~OnnxVisionBackend() = default;
bool OnnxVisionBackend::available() const {
#ifdef AETHERA_HAS_ONNX_RUNTIME
    return impl_ && impl_->session != nullptr;
#else
    return false;
#endif
}
bool OnnxVisionBackend::analyze(const ImageRgba8&, VisionResultLite& result) {
    result = {};
#ifdef AETHERA_HAS_ONNX_RUNTIME
    // Model-specific tensor layouts must be adapted by a model contract layer.
    // The backend is intentionally conservative until an explicit model schema is supplied.
    return false;
#else
    return false;
#endif
}
}
