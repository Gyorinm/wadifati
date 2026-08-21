#include "aethera/image_analyzer.hpp"

#include <algorithm>
#include <cmath>
#include <queue>
#include <string>
#include <utility>

namespace aethera {

namespace {

struct Pixel {
    int x;
    int y;
};

std::size_t index_of(int x, int y, int width) {
    return static_cast<std::size_t>(y) * static_cast<std::size_t>(width) +
           static_cast<std::size_t>(x);
}

bool near_color(const std::uint8_t* a, const std::uint8_t* b, std::uint8_t tolerance) {
    const int dr = std::abs(static_cast<int>(a[0]) - static_cast<int>(b[0]));
    const int dg = std::abs(static_cast<int>(a[1]) - static_cast<int>(b[1]));
    const int db = std::abs(static_cast<int>(a[2]) - static_cast<int>(b[2]));
    return dr <= tolerance && dg <= tolerance && db <= tolerance;
}

ImageRgba8 crop_region(const ImageRgba8& source,
                       const ImageRegion& region,
                       const std::vector<std::int32_t>& labels,
                       std::int32_t label) {
    const int x0 = std::max(0, static_cast<int>(std::floor(region.bounds.x)));
    const int y0 = std::max(0, static_cast<int>(std::floor(region.bounds.y)));
    const int x1 = std::min(source.width, x0 + static_cast<int>(std::ceil(region.bounds.width)));
    const int y1 = std::min(source.height, y0 + static_cast<int>(std::ceil(region.bounds.height)));

    ImageRgba8 result;
    result.width = std::max(0, x1 - x0);
    result.height = std::max(0, y1 - y0);
    result.pixels.assign(static_cast<std::size_t>(result.width) *
                         static_cast<std::size_t>(result.height) * 4u, 0u);

    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            if (labels[index_of(x, y, source.width)] != label) {
                continue;
            }
            const std::size_t src = index_of(x, y, source.width) * 4u;
            const std::size_t dst = index_of(x - x0, y - y0, result.width) * 4u;
            result.pixels[dst + 0] = source.pixels[src + 0];
            result.pixels[dst + 1] = source.pixels[src + 1];
            result.pixels[dst + 2] = source.pixels[src + 2];
            result.pixels[dst + 3] = source.pixels[src + 3];
        }
    }

    return result;
}

} // namespace

ImageAnalysisResult ImageAnalyzer::analyze(const ImageRgba8& image,
                                            const ImageAnalysisOptions& options) const {
    ImageAnalysisResult result;
    if (!image.valid()) {
        return result;
    }

    const int width = image.width;
    const int height = image.height;
    const std::size_t pixel_total = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);

    result.labels.assign(pixel_total, -1);

    std::uint8_t background[4] = {0, 0, 0, 255};
    bool have_background = false;
    if (options.detect_background && image.pixels.size() >= 4) {
        const int samples[][2] = {{0, 0}, {width - 1, 0}, {0, height - 1}, {width - 1, height - 1}};
        long sum_r = 0;
        long sum_g = 0;
        long sum_b = 0;
        int count = 0;
        for (const auto& sample : samples) {
            const std::size_t p = index_of(sample[0], sample[1], width) * 4u;
            if (image.pixels[p + 3] >= options.alpha_threshold) {
                sum_r += image.pixels[p + 0];
                sum_g += image.pixels[p + 1];
                sum_b += image.pixels[p + 2];
                ++count;
            }
        }
        if (count > 0) {
            background[0] = static_cast<std::uint8_t>(sum_r / count);
            background[1] = static_cast<std::uint8_t>(sum_g / count);
            background[2] = static_cast<std::uint8_t>(sum_b / count);
            background[3] = 255;
            have_background = true;
        }
    }

    std::vector<std::uint8_t> foreground(pixel_total, 0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::size_t p = index_of(x, y, width) * 4u;
            const auto* rgba = image.pixels.data() + p;
            const bool alpha_foreground = rgba[3] >= options.alpha_threshold;
            const bool color_foreground = !have_background || !near_color(rgba, background, options.background_tolerance);
            foreground[index_of(x, y, width)] = (alpha_foreground && color_foreground) ? 1 : 0;
        }
    }

    constexpr int dx[] = {1, -1, 0, 0};
    constexpr int dy[] = {0, 0, 1, -1};
    std::int32_t next_label = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::size_t seed_index = index_of(x, y, width);
            if (!foreground[seed_index] || result.labels[seed_index] != -1) {
                continue;
            }

            ImageRegion region;
            std::queue<Pixel> queue;
            queue.push({x, y});
            result.labels[seed_index] = next_label;

            long sum_x = 0;
            long sum_y = 0;
            long sum_alpha = 0;
            int min_x = x;
            int max_x = x;
            int min_y = y;
            int max_y = y;

            while (!queue.empty()) {
                const Pixel current = queue.front();
                queue.pop();

                const std::size_t current_index = index_of(current.x, current.y, width);
                const std::size_t p = current_index * 4u;
                region.pixel_count += 1;
                sum_x += current.x;
                sum_y += current.y;
                sum_alpha += image.pixels[p + 3];
                min_x = std::min(min_x, current.x);
                max_x = std::max(max_x, current.x);
                min_y = std::min(min_y, current.y);
                max_y = std::max(max_y, current.y);

                for (int n = 0; n < 4; ++n) {
                    const int nx = current.x + dx[n];
                    const int ny = current.y + dy[n];
                    if (nx < 0 || ny < 0 || nx >= width || ny >= height) {
                        continue;
                    }
                    const std::size_t ni = index_of(nx, ny, width);
                    if (foreground[ni] && result.labels[ni] == -1) {
                        result.labels[ni] = next_label;
                        queue.push({nx, ny});
                    }
                }
            }

            region.bounds = Rect{
                static_cast<float>(min_x),
                static_cast<float>(min_y),
                static_cast<float>(max_x - min_x + 1),
                static_cast<float>(max_y - min_y + 1)
            };
            region.centroid = Vec2{
                static_cast<float>(sum_x) / static_cast<float>(region.pixel_count),
                static_cast<float>(sum_y) / static_cast<float>(region.pixel_count)
            };
            region.average_alpha = static_cast<std::uint8_t>(sum_alpha / static_cast<long>(region.pixel_count));
            result.regions.push_back(region);
            ++next_label;
        }
    }

    if (options.min_region_pixels > 1 && !result.regions.empty()) {
        std::vector<std::int32_t> remap(result.regions.size(), -1);
        std::vector<ImageRegion> filtered;
        filtered.reserve(result.regions.size());
        for (std::size_t i = 0; i < result.regions.size(); ++i) {
            if (result.regions[i].pixel_count < options.min_region_pixels) {
                continue;
            }
            remap[i] = static_cast<std::int32_t>(filtered.size());
            filtered.push_back(result.regions[i]);
        }
        for (auto& label : result.labels) {
            if (label >= 0 && static_cast<std::size_t>(label) < remap.size()) {
                label = remap[static_cast<std::size_t>(label)];
            }
        }
        result.regions = std::move(filtered);
    }

    result.background_label = -1;
    return result;
}

ImageObject ImageAnalyzer::create_object(const ImageRgba8& image,
                                          const ImageAnalysisOptions& options) const {
    ImageObject object("analyzed_image");
    object.set_image(&image);

    const ImageAnalysisResult analysis = analyze(image, options);
    for (std::size_t i = 0; i < analysis.regions.size(); ++i) {
        const ImageRegion& region = analysis.regions[i];
        ImageNode node;
        node.name = "region_" + std::to_string(i);
        node.parent = static_cast<std::size_t>(-1);
        node.visual_image = object.add_image(crop_region(image, region, analysis.labels, static_cast<std::int32_t>(i)));
        node.visual.name = node.name;
        node.visual.source = Rect{0.0f, 0.0f, region.bounds.width, region.bounds.height};
        node.visual.pivot = Vec2{
            region.centroid.x - region.bounds.x,
            region.centroid.y - region.bounds.y
        };
        node.local.position = region.centroid;
        object.add_node(std::move(node));
    }

    object.update_world_transforms();
    return object;
}

} // namespace aethera
