#include "aethera/image_object.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace aethera {

namespace {

ImageTransform combine(const ImageTransform& parent, const ImageTransform& local) {
    const float c = std::cos(parent.rotation);
    const float s = std::sin(parent.rotation);

    const Vec2 scaled{
        local.position.x * parent.scale.x,
        local.position.y * parent.scale.y
    };

    ImageTransform result;
    result.position = parent.position + Vec2{
        scaled.x * c - scaled.y * s,
        scaled.x * s + scaled.y * c
    };
    result.rotation = parent.rotation + local.rotation;
    result.scale = {
        parent.scale.x * local.scale.x,
        parent.scale.y * local.scale.y
    };
    return result;
}

} // namespace

ImageObject::ImageObject(std::string name) : name_(std::move(name)) {}

std::size_t ImageObject::add_image(ImageRgba8 image) {
    node_images_.push_back(std::move(image));
    return node_images_.size() - 1;
}

const ImageRgba8* ImageObject::image_for_node(std::size_t index) const {
    if (index >= nodes_.size()) {
        return nullptr;
    }
    const std::size_t image_index = nodes_[index].visual_image;
    if (image_index == static_cast<std::size_t>(-1) || image_index >= node_images_.size()) {
        return image_;
    }
    return &node_images_[image_index];
}

std::size_t ImageObject::add_node(ImageNode node) {
    if (node.parent >= nodes_.size()) {
        node.parent = static_cast<std::size_t>(-1);
    }

    nodes_.push_back(std::move(node));
    world_.resize(nodes_.size());
    return nodes_.size() - 1;
}

ImageNode* ImageObject::node(std::size_t index) {
    return index < nodes_.size() ? &nodes_[index] : nullptr;
}

const ImageNode* ImageObject::node(std::size_t index) const {
    return index < nodes_.size() ? &nodes_[index] : nullptr;
}

void ImageObject::update_world_transforms() {
    world_.resize(nodes_.size());

    const ImageTransform identity{};
    for (std::size_t i = 0; i < nodes_.size(); ++i) {
        if (nodes_[i].parent == static_cast<std::size_t>(-1)) {
            update_node(i, identity);
        }
    }
}

void ImageObject::update_node(std::size_t index, const ImageTransform& parent_world) {
    if (index >= nodes_.size()) {
        return;
    }

    world_[index] = combine(parent_world, nodes_[index].local);

    for (std::size_t child = 0; child < nodes_.size(); ++child) {
        if (nodes_[child].parent == index) {
            update_node(child, world_[index]);
        }
    }
}

const ImageTransform& ImageObject::world_transform(std::size_t index) const {
    if (index >= world_.size()) {
        throw std::out_of_range("Aethera image node index out of range");
    }
    return world_[index];
}

} // namespace aethera
