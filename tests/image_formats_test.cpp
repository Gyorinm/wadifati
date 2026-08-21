#include "aethera/assets.hpp"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace {

void write_u16(std::ofstream& out, std::uint16_t v) {
    const char b[2] = {static_cast<char>(v & 0xffu), static_cast<char>((v >> 8) & 0xffu)};
    out.write(b, 2);
}

void write_u32(std::ofstream& out, std::uint32_t v) {
    const char b[4] = {
        static_cast<char>(v & 0xffu), static_cast<char>((v >> 8) & 0xffu),
        static_cast<char>((v >> 16) & 0xffu), static_cast<char>((v >> 24) & 0xffu)};
    out.write(b, 4);
}

void write_test_bmp(const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    assert(out);

    // 2x1, 24-bit BGR, one padded row.
    const std::uint32_t file_size = 14 + 40 + 8;
    out.put('B'); out.put('M');
    write_u32(out, file_size);
    write_u16(out, 0); write_u16(out, 0); write_u32(out, 54);
    write_u32(out, 40); write_u32(out, 2); write_u32(out, 1);
    write_u16(out, 1); write_u16(out, 24); write_u32(out, 0);
    write_u32(out, 8); write_u32(out, 0); write_u32(out, 0); write_u32(out, 0); write_u32(out, 0);

    // Pixel 0 = red, pixel 1 = green, then 2 bytes of row padding.
    const unsigned char row[8] = {0, 0, 255, 0, 255, 0, 0, 0};
    out.write(reinterpret_cast<const char*>(row), sizeof(row));
}

} // namespace

int main() {
    const std::string ppm = "aethera_image_formats_test.ppm";
    {
        std::ofstream out(ppm, std::ios::binary);
        out << "P6\n2 1\n255\n";
        const unsigned char rgb[6] = {255, 0, 0, 0, 255, 0};
        out.write(reinterpret_cast<const char*>(rgb), sizeof(rgb));
    }

    aethera::ImageAsset ppm_asset;
    assert(ppm_asset.load(ppm));
    assert(ppm_asset.format() == aethera::ImageFileFormat::Ppm);
    assert(ppm_asset.image().width == 2);
    assert(ppm_asset.image().height == 1);
    assert(ppm_asset.image().pixels[3] == 255);

    const std::string bmp = "aethera_image_formats_test.bmp";
    write_test_bmp(bmp);
    aethera::ImageAsset bmp_asset;
    assert(bmp_asset.load(bmp));
    assert(bmp_asset.format() == aethera::ImageFileFormat::Bmp);
    assert(bmp_asset.image().width == 2);
    assert(bmp_asset.image().height == 1);
    assert(bmp_asset.image().pixels[0] == 255);
    assert(bmp_asset.image().pixels[4] == 255);

    aethera::AssetManager assets;
    auto first = assets.load_image(ppm);
    auto second = assets.load_image(ppm);
    assert(first);
    assert(first == second);
    assert(assets.image_count() == 1);

    std::remove(ppm.c_str());
    std::remove(bmp.c_str());
    return 0;
}
