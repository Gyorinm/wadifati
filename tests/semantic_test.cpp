#include "aethera/semantic.hpp"

#include <cassert>
#include <iostream>

int main() {
    aethera::ImageAnalysisResult analysis;
    analysis.labels.assign(100 * 100, -1);

    analysis.regions.push_back({{20.0f, 10.0f, 60.0f, 80.0f}, 4800, {50.0f, 50.0f}, 255, aethera::ImageObjectKind::Region});
    analysis.regions.push_back({{5.0f, 5.0f, 10.0f, 10.0f}, 100, {10.0f, 10.0f}, 255, aethera::ImageObjectKind::Region});
    analysis.regions.push_back({{80.0f, 5.0f, 10.0f, 10.0f}, 100, {85.0f, 10.0f}, 255, aethera::ImageObjectKind::Region});
    analysis.regions.push_back({{10.0f, 80.0f, 12.0f, 12.0f}, 144, {16.0f, 86.0f}, 255, aethera::ImageObjectKind::Region});
    analysis.regions.push_back({{78.0f, 80.0f, 12.0f, 12.0f}, 144, {84.0f, 86.0f}, 255, aethera::ImageObjectKind::Region});
    analysis.regions.push_back({{45.0f, 0.0f, 10.0f, 8.0f}, 80, {50.0f, 4.0f}, 255, aethera::ImageObjectKind::Region});

    aethera::SemanticAnalyzer analyzer;
    auto semantic = analyzer.classify(analysis);
    analyzer.infer_skeleton(analysis, semantic);

    assert(semantic.kind == aethera::SemanticKind::Human);
    assert(semantic.joints.size() == analysis.regions.size());
    assert(semantic.links.size() == semantic.joints.size() - 1);
    assert(semantic.confidence > 0.0f);

    std::cout << "semantic test passed: "
              << aethera::semantic_kind_name(semantic.kind)
              << ", joints=" << semantic.joints.size()
              << ", links=" << semantic.links.size() << '\n';
    return 0;
}
