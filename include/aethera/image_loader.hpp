#pragma once
#include <string>
#include "aethera/image.hpp"
namespace aethera {
class ImageLoader {
public:
    static bool load(const std::string& path, ImageRgba8& out);
};
}
