#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "aethera/image_analyzer.hpp"
#include "aethera/semantic.hpp"

namespace aethera {

enum class VisionBackend {
    Heuristic,
    ExternalModel
};

struct VisionOptions {
    ImageAnalysisOptions segmentation{};
    bool infer_semantics{true};
    bool infer_skeleton{true};
};

struct VisionResult {
    ImageAnalysisResult analysis{};
    SemanticObject semantic{};
    std::vector<std::string> diagnostics;

    bool valid() const { return analysis.valid(); }
};

class VisionBackendInterface {
public:
    virtual ~VisionBackendInterface() = default;
    virtual VisionBackend backend() const = 0;
    virtual VisionResult analyze(const ImageRgba8& image,
                                  const VisionOptions& options = {}) const = 0;
};

class HeuristicVisionBackend final : public VisionBackendInterface {
public:
    VisionBackend backend() const override { return VisionBackend::Heuristic; }
    VisionResult analyze(const ImageRgba8& image,
                         const VisionOptions& options = {}) const override;
};

class VisionSystem {
public:
    VisionSystem();
    explicit VisionSystem(std::unique_ptr<VisionBackendInterface> backend);

    void set_backend(std::unique_ptr<VisionBackendInterface> backend);
    const VisionBackendInterface& backend() const;
    VisionResult analyze(const ImageRgba8& image,
                         const VisionOptions& options = {}) const;

private:
    std::unique_ptr<VisionBackendInterface> backend_;
};

const char* vision_backend_name(VisionBackend backend);

} // namespace aethera
