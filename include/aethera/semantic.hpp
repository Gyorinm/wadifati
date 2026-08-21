#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "aethera/image_analyzer.hpp"
#include "aethera/image_object.hpp"

namespace aethera {

enum class SemanticKind {
    Unknown,
    Human,
    Quadruped,
    Insect,
    Fish,
    Amphibian,
    Plant,
    Tree,
    Building,
    Vehicle,
    Landscape,
    Water
};

struct SemanticJoint {
    std::string name;
    Vec2 position{};
    std::size_t source_region{static_cast<std::size_t>(-1)};
};

struct SemanticLink {
    std::size_t a{0};
    std::size_t b{0};
    float rest_length{0.0f};
};

struct SemanticObject {
    SemanticKind kind{SemanticKind::Unknown};
    float confidence{0.0f};
    std::vector<std::size_t> regions;
    std::vector<SemanticJoint> joints;
    std::vector<SemanticLink> links;
};

class SemanticAnalyzer {
public:
    SemanticObject classify(const ImageAnalysisResult& analysis) const;
    void infer_skeleton(const ImageAnalysisResult& analysis, SemanticObject& object) const;
    ImageObject build_image_object(const ImageRgba8& image,
                                   const ImageAnalysisResult& analysis,
                                   const SemanticObject& semantic) const;
};

const char* semantic_kind_name(SemanticKind kind);

} // namespace aethera
