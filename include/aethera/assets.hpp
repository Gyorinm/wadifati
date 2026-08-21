#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "aethera/image.hpp"

namespace aethera {

class AssetManager {
public:
    std::shared_ptr<ImageAsset> load_image(const std::string& path);
    void clear();
    std::size_t image_count() const { return images_.size(); }

private:
    std::unordered_map<std::string, std::shared_ptr<ImageAsset>> images_;
};

} // namespace aethera
