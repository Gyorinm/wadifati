#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "aethera/math.hpp"

namespace aethera {

enum class PartKind {
    Point,
    Limb,
    Surface
};

struct Part {
    std::string name;
    PartKind kind{PartKind::Point};
    std::size_t physics_point{0};
    float radius{6.0f};
    float thickness{4.0f};
};

class LivingObject {
public:
    explicit LivingObject(std::string name);

    const std::string& name() const { return name_; }

    std::size_t add_part(Part part);
    void add_bone(std::size_t a, std::size_t b);

    std::vector<Part>& parts() { return parts_; }
    const std::vector<Part>& parts() const { return parts_; }
    const std::vector<std::pair<std::size_t, std::size_t>>& bones() const { return bones_; }

private:
    std::string name_;
    std::vector<Part> parts_;
    std::vector<std::pair<std::size_t, std::size_t>> bones_;
};

} // namespace aethera
