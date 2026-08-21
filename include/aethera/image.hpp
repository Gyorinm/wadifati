#pragma once
#include <cstdint>
#include <vector>
namespace aethera {
struct ImageRgba8 {
    int width{0}, height{0};
    std::vector<std::uint8_t> pixels;
    bool valid() const { return width>0 && height>0 && pixels.size()==static_cast<std::size_t>(width*height*4); }
};
}
