#pragma once
#include <string>
#include "aethera/image_pipeline.hpp"
namespace aethera {
class SceneFile {
public:
    static bool save(const std::string& path, const ImageActor& actor);
    static bool load(const std::string& path, ImageActor& actor);
};
}
