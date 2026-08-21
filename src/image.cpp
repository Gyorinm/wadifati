#include "aethera/image.hpp"

#include <fstream>
#include <string>

namespace aethera {

bool ImageRgba8::valid() const {
    return width > 0 && height > 0 &&
           pixels.size() == static_cast<std::size_t>(width * height * 4);
}

namespace {

bool next_token(std::istream& in, std::string& token) {
    while (in >> token) {
        if (!token.empty() && token[0] == '#') {
            std::string ignored;
            std::getline(in, ignored);
            continue;
        }
        return true;
    }
    return false;
}

} // namespace

bool ImageAsset::load_ppm(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    std::string magic;
    if (!next_token(file, magic) || (magic != "P6" && magic != "P3")) return false;

    std::string token;
    if (!next_token(file, token)) return false;
    const int width = std::stoi(token);
    if (!next_token(file, token)) return false;
    const int height = std::stoi(token);
    if (!next_token(file, token)) return false;
    const int max_value = std::stoi(token);
    if (width <= 0 || height <= 0 || max_value <= 0 || max_value > 255) return false;

    ImageRgba8 next;
    next.width = width;
    next.height = height;
    next.pixels.resize(static_cast<std::size_t>(width * height * 4));

    if (magic == "P6") {
        file.get();
        std::vector<std::uint8_t> rgb(static_cast<std::size_t>(width * height * 3));
        if (!file.read(reinterpret_cast<char*>(rgb.data()), static_cast<std::streamsize>(rgb.size()))) {
            return false;
        }
        for (std::size_t i = 0, p = 0; i < rgb.size(); i += 3, p += 4) {
            next.pixels[p + 0] = rgb[i + 0];
            next.pixels[p + 1] = rgb[i + 1];
            next.pixels[p + 2] = rgb[i + 2];
            next.pixels[p + 3] = 255;
        }
    } else {
        for (std::size_t p = 0; p < next.pixels.size(); p += 4) {
            for (int c = 0; c < 3; ++c) {
                if (!next_token(file, token)) return false;
                const int value = std::stoi(token);
                if (value < 0 || value > max_value) return false;
                next.pixels[p + static_cast<std::size_t>(c)] =
                    static_cast<std::uint8_t>((value * 255) / max_value);
            }
            next.pixels[p + 3] = 255;
        }
    }

    image_ = std::move(next);
    source_path_ = path;
    return true;
}

} // namespace aethera
