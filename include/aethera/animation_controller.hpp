#pragma once
#include "aethera/image_pipeline.hpp"
namespace aethera {
class AnimationController {
public:
    static void idle(ImageActor& actor, float time);
    static void sway(ImageActor& actor, float time, float amplitude, float frequency);
    static void reset(ImageActor& actor);
};
}
