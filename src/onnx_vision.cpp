#include "aethera/onnx_vision.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#ifdef AETHERA_HAS_ONNX_RUNTIME
#include <onnxruntime_cxx_api.h>
#endif

namespace aethera {

namespace {

#ifdef AETHERA_HAS_ONNX_RUNTIME
struct SessionHolder {
    Ort::Env env{ORT_LOGGING_LEVEL_WARNING, "aethera"};
    Ort::SessionOptions options{};
    std::unique_ptr<Ort::Session> session;

    explicit SessionHolder(const std::string& path) {
        options.SetIntraOpNumThreads(1);
        options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
        session = std::make_unique<Ort::Session>(env, path.c_str(), options);
    }
};
#endif

} // namespace

OnnxVisionBackend::OnnxVisionBackend(OnnxVisionModelConfig config)
    : config_(std::move(config)) {
#ifdef AETHERA_HAS_ONNX_RUNTIME
    try {
        auto* holder = new SessionHolder(config_.model_path);
        session_ = holder;
    } catch (const std::exception& ex) {
        error_ = ex.what();
        session_ = nullptr;
    }
#else
    error_ = "ONNX Runtime is not available in this build";
#endif
}

OnnxVisionBackend::~OnnxVisionBackend() {
#ifdef AETHERA_HAS_ONNX_RUNTIME
    delete static_cast<SessionHolder*>(session_);
#endif
    session_ = nullptr;
}

bool OnnxVisionBackend::available() const {
#ifdef AETHERA_HAS_ONNX_RUNTIME
    return session_ != nullptr && error_.empty();
#else
    return false;
#endif
}

VisionResult OnnxVisionBackend::analyze(const ImageRgba8& image,
                                        const VisionOptions& options) const {
    VisionResult result;
    if (!image.valid()) {
        result.diagnostics.push_back("invalid image");
        return result;
    }

#ifdef AETHERA_HAS_ONNX_RUNTIME
    if (!available()) {
        result.diagnostics.push_back(error_.empty() ? "ONNX session unavailable" : error_);
        return result;
    }

    try {
        auto* holder = static_cast<SessionHolder*>(session_);
        Ort::AllocatorWithDefaultOptions allocator;
        const std::size_t h = config_.input_height;
        const std::size_t w = config_.input_width;
        std::vector<float> input(3 * h * w, 0.0f);

        for (std::size_t y = 0; y < h; ++y) {
            const std::size_t sy = std::min<std::size_t>(image.height - 1, y * static_cast<std::size_t>(image.height) / h);
            for (std::size_t x = 0; x < w; ++x) {
                const std::size_t sx = std::min<std::size_t>(image.width - 1, x * static_cast<std::size_t>(image.width) / w);
                const std::size_t p = (sy * static_cast<std::size_t>(image.width) + sx) * 4;
                input[(0 * h + y) * w + x] = image.pixels[p + 0] / 255.0f;
                input[(1 * h + y) * w + x] = image.pixels[p + 1] / 255.0f;
                input[(2 * h + y) * w + x] = image.pixels[p + 2] / 255.0f;
            }
        }

        const std::array<int64_t, 4> shape = {1, 3, static_cast<int64_t>(h), static_cast<int64_t>(w)};
        Ort::MemoryInfo memory = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory, input.data(), input.size(), shape.data(), shape.size());

        const char* input_name = holder->session->GetInputNameAllocated(0, allocator).get();
        const std::size_t output_count = holder->session->GetOutputCount();
        std::vector<Ort::AllocatedStringPtr> output_names;
        output_names.reserve(output_count);
        std::vector<const char*> output_name_ptrs;
        output_name_ptrs.reserve(output_count);
        for (std::size_t i = 0; i < output_count; ++i) {
            output_names.push_back(holder->session->GetOutputNameAllocated(i, allocator));
            output_name_ptrs.push_back(output_names.back().get());
        }

        auto outputs = holder->session->Run(Ort::RunOptions{nullptr}, &input_name, &input_tensor, 1,
                                            output_name_ptrs.data(), output_name_ptrs.size());

        // This adapter deliberately treats output decoding as a model contract.
        // The first output can optionally provide a foreground mask; otherwise
        // we fall back to the lightweight segmentation so every model remains useful.
        if (!outputs.empty() && outputs[0].IsTensor()) {
            const auto info = outputs[0].GetTensorTypeAndShapeInfo();
            const auto dims = info.GetShape();
            if (dims.size() == 4 && dims[0] == 1 && dims[1] == 1 &&
                dims[2] > 0 && dims[3] > 0) {
                const float* mask = outputs[0].GetTensorData<float>();
                ImageRgba8 masked = image;
                for (std::size_t y = 0; y < static_cast<std::size_t>(image.height); ++y) {
                    for (std::size_t x = 0; x < static_cast<std::size_t>(image.width); ++x) {
                        const std::size_t my = std::min<std::size_t>(static_cast<std::size_t>(dims[2] - 1), y * static_cast<std::size_t>(dims[2]) / static_cast<std::size_t>(image.height));
                        const std::size_t mx = std::min<std::size_t>(static_cast<std::size_t>(dims[3] - 1), x * static_cast<std::size_t>(dims[3]) / static_cast<std::size_t>(image.width));
                        const float score = mask[my * static_cast<std::size_t>(dims[3]) + mx];
                        if (score < config_.mask_threshold) masked.pixels[(y * static_cast<std::size_t>(image.width) + x) * 4 + 3] = 0;
                    }
                }
                ImageAnalyzer analyzer;
                result.analysis = analyzer.analyze(masked, options.segmentation);
                result.diagnostics.push_back("ONNX foreground mask applied");
            }
        }

        if (!result.analysis.valid()) {
            ImageAnalyzer analyzer;
            result.analysis = analyzer.analyze(image, options.segmentation);
            result.diagnostics.push_back("ONNX model produced no usable mask; heuristic segmentation used");
        }

        SemanticAnalyzer semantic;
        if (options.infer_semantics) result.semantic = semantic.classify(result.analysis);
        if (options.infer_skeleton && options.infer_semantics) semantic.infer_skeleton(result.analysis, result.semantic);
        result.diagnostics.push_back("ONNX vision analysis completed");
        return result;
    } catch (const std::exception& ex) {
        result.diagnostics.push_back(std::string("ONNX inference failed: ") + ex.what());
        return result;
    }
#else
    result.diagnostics.push_back(error_);
    (void)options;
    return result;
#endif
}

} // namespace aethera
