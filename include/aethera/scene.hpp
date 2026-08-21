#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "aethera/image_object.hpp"

namespace aethera {

struct SceneNodeData {
    std::string name;
    std::size_t parent{static_cast<std::size_t>(-1)};
    bool visible{true};
    ImageTransform transform{};
    ImagePart visual{};
};

struct Scene {
    std::string name;
    std::string image_asset;
    std::vector<SceneNodeData> nodes;
};

class SceneSerializer {
public:
    bool save(const Scene& scene, const std::string& path, std::string* error = nullptr) const;
    bool load(const std::string& path, Scene& scene, std::string* error = nullptr) const;
};

Scene capture_scene(const ImageObject& object, const std::string& name,
                    const std::string& image_asset = {});

bool apply_scene(const Scene& scene, ImageObject& object);

} // namespace aethera
