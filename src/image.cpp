#include "aethera/image.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#if defined(AETHERA_HAS_SDL_IMAGE)
#include <SDL_image.h>
#endif

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

std::string lower_extension(const std::string& path) {
    const auto slash = path.find_last_of("/\\");
    const auto dot = path.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) return {};
    std::string ext = path.substr(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

std::uint16_t read_u16(std::istream& in) {
    unsigned char b[2]{};
    if (!in.read(reinterpret_cast<char*>(b), 2)) return 0;
    return static_cast<std::uint16_t>(b[0] | (static_cast<std::uint16_t>(b[1]) << 8));
}

std::uint32_t read_u32(std::istream& in) {
    unsigned char b[4]{};
    if (!in.read(reinterpret_cast<char*>(b), 4)) return 0;
    return static_cast<std::uint32_t>(b[0] |
        (static_cast<std::uint32_t>(b[1]) << 8) |
        (static_cast<std::uint32_t>(b[2]) << 16) |
        (static_cast<std::uint32_t>(b[3]) << 24));
}

std::int32_t read_i32(std::istream& in) {
    return static_cast<std::int32_t>(read_u32(in));
}

#if defined(AETHERA_HAS_SDL_IMAGE)
bool load_with_sdl_image(const std::string& path, ImageRgba8& out) {
    SDL_Surface* source = IMG_Load(path.c_str());
    if (source == nullptr) return false;

    SDL_Surface* rgba = SDL_ConvertSurfaceFormat(source, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(source);
    if (rgba == nullptr) return false;

    out.width = rgba->w;
    out.height = rgba->h;
    out.pixels.resize(static_cast<std::size_t>(out.width * out.height * 4));

    const auto* src = static_cast<const std::uint8_t*>(rgba->pixels);
    for (int y = 0; y < out.height; ++y) {
        const auto* row = src + static_cast<std::size_t>(y) * rgba->pitch;
        auto* dst = out.pixels.data() + static_cast<std::size_t>(y * out.width * 4);
        std::memcpy(dst, row, static_cast<std::size_t>(out.width * 4));
    }

    SDL_FreeSurface(rgba);
    return out.valid();
}
#endif

} // namespace

bool ImageAsset::load(const std::string& path) {
    const std::string ext = lower_extension(path);

    if (ext == ".ppm" || ext == ".pnm") return load_ppm(path);
    if (ext == ".bmp") return load_bmp(path);

#if defined(AETHERA_HAS_SDL_IMAGE)
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".webp") {
        if (!load_with_sdl_image(path, image_)) return false;
        source_path_ = path;
        if (ext == ".png") format_ = ImageFileFormat::Png;
        else if (ext == ".webp") format_ = ImageFileFormat::Webp;
        else format_ = ImageFileFormat::Jpeg;
        return true;
    }
#endif

    return false;
}

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
    format_ = ImageFileFormat::Ppm;
    return true;
}

bool ImageAsset::load_bmp(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return false;

    char signature[2]{};
    if (!file.read(signature, 2) || signature[0] != 'B' || signature[1] != 'M') return false;

    (void)read_u32(file);
    (void)read_u16(file);
    (void)read_u16(file);
    const std::uint32_t pixel_offset = read_u32(file);
    const std::uint32_t dib_size = read_u32(file);
    if (dib_size < 40) return false;

    const std::int32_t width = read_i32(file);
    const std::int32_t signed_height = read_i32(file);
    const std::uint16_t planes = read_u16(file);
    const std::uint16_t bits = read_u16(file);
    const std::uint32_t compression = read_u32(file);
    (void)read_u32(file);
    (void)read_i32(file);
    (void)read_i32(file);
    (void)read_u32(file);
    (void)read_u32(file);

    if (width <= 0 || signed_height == 0 || planes != 1 || compression != 0 ||
        (bits != 24 && bits != 32)) return false;

    const int height = signed_height < 0 ? -signed_height : signed_height;
    const bool top_down = signed_height < 0;
    const std::size_t bytes_per_pixel = bits / 8;
    const std::size_t row_stride = ((static_cast<std::size_t>(width) * bytes_per_pixel + 3) / 4) * 4;

    ImageRgba8 next;
    next.width = width;
    next.height = height;
    next.pixels.resize(static_cast<std::size_t>(width * height * 4));

    file.seekg(static_cast<std::streamoff>(pixel_offset), std::ios::beg);
    std::vector<std::uint8_t> row(row_stride);
    for (int y = 0; y < height; ++y) {
        if (!file.read(reinterpret_cast<char*>(row.data()), static_cast<std::streamsize>(row.size()))) {
            return false;
        }
        const int dst_y = top_down ? y : (height - 1 - y);
        for (int x = 0; x < width; ++x) {
            const std::size_t src = static_cast<std::size_t>(x) * bytes_per_pixel;
            const std::size_t dst = static_cast<std::size_t>((dst_y * width + x) * 4);
            next.pixels[dst + 0] = row[src + 2];
            next.pixels[dst + 1] = row[src + 1];
            next.pixels[dst + 2] = row[src + 0];
            next.pixels[dst + 3] = bits == 32 ? row[src + 3] : 255;
        }
    }

    image_ = std::move(next);
    source_path_ = path;
    format_ = ImageFileFormat::Bmp;
    return true;
}

} // namespace aethera
