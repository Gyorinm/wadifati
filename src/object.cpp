#include "aethera/object.hpp"

namespace aethera {

LivingObject::LivingObject(std::string name) : name_(std::move(name)) {}

std::size_t LivingObject::add_part(Part part) {
    parts_.push_back(std::move(part));
    return parts_.size() - 1;
}

void LivingObject::add_bone(std::size_t a, std::size_t b) {
    bones_.emplace_back(a, b);
}

} // namespace aethera
