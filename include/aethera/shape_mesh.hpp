#pragma once
#include <cstdint>
#include <vector>
#include "aethera/image.hpp"
#include "aethera/mesh.hpp"
namespace aethera {
class ShapeMeshGenerator {
public:
    static std::vector<MeshVertex> vertices_from_alpha(const ImageRgba8& image, int columns, int rows, std::uint8_t alpha_threshold=8);
    static std::vector<std::uint32_t> indices(int columns, int rows);
};
}
