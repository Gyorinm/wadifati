#include "aethera/vision.hpp"

#include <memory>

namespace aethera {

VisionResult HeuristicVisionBackend::analyze(const ImageRgba8& image,
                                             const VisionOptions& options) const {
    VisionResult result;
    ImageAnalyzer analyzer;
    result.analysis = analyzer.analyze(image, options.segmentation);
    if (!result.analysis.valid()) {
        result.diagnostics.push_back("segmentation produced no usable regions");
        return result;
    }

    SemanticAnalyzer semantic;
    if (options.infer_semantics) {
        result.semantic = semantic.classify(result.analysis);
    }
    if (options.infer_skeleton && options.infer_semantics) {
        semantic.infer_skeleton(result.analysis, result.semantic);
    }

    result.diagnostics.push_back("heuristic segmentation completed");
    if (options.infer_semantics) result.diagnostics.push_back("heuristic semantic classification completed");
    if (options.infer_skeleton) result.diagnostics.push_back("heuristic skeleton inference completed");
    return result;
}

VisionSystem::VisionSystem()
    : backend_(std::make_unique<HeuristicVisionBackend>()) {}

VisionSystem::VisionSystem(std::unique_ptr<VisionBackendInterface> backend)
    : backend_(std::move(backend)) {
    if (!backend_) backend_ = std::make_unique<HeuristicVisionBackend>();
}

void VisionSystem::set_backend(std::unique_ptr<VisionBackendInterface> backend) {
    backend_ = std::move(backend);
    if (!backend_) backend_ = std::make_unique<HeuristicVisionBackend>();
}

const VisionBackendInterface& VisionSystem::backend() const {
    return *backend_;
}

VisionResult VisionSystem::analyze(const ImageRgba8& image,
                                   const VisionOptions& options) const {
    return backend().analyze(image, options);
}

const char* vision_backend_name(VisionBackend backend) {
    switch (backend) {
        case VisionBackend::Heuristic: return "heuristic";
        case VisionBackend::ExternalModel: return "external_model";
        default: return "unknown";
    }
}

} // namespace aethera
