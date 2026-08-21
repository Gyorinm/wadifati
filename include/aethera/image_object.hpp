#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "aethera/image.hpp"
#include "aethera/math.hpp"

namespace aethera {

enum class ImageTransformSpace {
    Local,
    World
};

struct ImageTransform {
    Vec2 position{};
    float rotation{0.0f};
    Vec2 scale{1.0f, 1.0f};
};

struct ImageNode {
    std::string name;
    std::size_t parent{static_cast<std::size_t>(-1)};
    std::size_t visual_image{static_cast<std::size_t>(-1)};
    ImagePart visual{};
    ImageTransform local{};
    bool visible{true};
};

class ImageObject {
public:
    explicit ImageObject(std::string name = {});

    const std::string& name() const { return name_; }

    void set_image(const ImageRgba8* image) { image_ = image; }
    const ImageRgba8* image() const { return image_; }

    std::size_t add_image(ImageRgba8 image);
    const ImageRgba8* image_for_node(std::size_t index) const;

    std::size_t add_node(ImageNode node);
    ImageNode* node(std::size_t index);
    const ImageNode* node(std::size_t index) const;

    std::vector<ImageNode>& nodes() { return nodes_; }
    const std::vector<ImageNode>& nodes() const { return nodes_; }

    void update_world_transforms();
    const ImageTransform& world_transform(std::size_t index) const;

private:
    void update_node(std::size_t index, const ImageTransform& parent_world);

    std::string name_;
    const ImageRgba8* image_{nullptr};
    std::vector<ImageRgba8> node_images_;
    std::vector<ImageNode> nodes_;
    std::vector<ImageTransform> world_;
};

} // namespace aethera
