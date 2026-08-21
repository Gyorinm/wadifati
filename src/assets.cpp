#include "aethera/assets.hpp"

namespace aethera {

std::shared_ptr<ImageAsset> AssetManager::load_image(const std::string& path) {
    const auto cached = images_.find(path);
    if (cached != images_.end()) return cached->second;

    auto asset = std::make_shared<ImageAsset>();
    if (!asset->load(path)) return {};
    images_.emplace(path, asset);
    return asset;
}

void AssetManager::clear() {
    images_.clear();
}

} // namespace aethera
