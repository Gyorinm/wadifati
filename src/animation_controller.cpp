#include "aethera/animation_controller.hpp"
#include <cmath>
namespace aethera {
void AnimationController::idle(ImageActor& a, float t) {
    if (a.skeleton.empty()) return;
    a.skeleton[0].current = a.skeleton[0].bind;
    a.skeleton[1].current = a.skeleton[1].bind + Vec2{0.0f, std::sin(t * 2.0f) * 2.0f};
    a.skeleton[1].current_rotation = std::sin(t * 2.0f) * 0.05f;
    ImagePipeline::update_bones(a);
}
void AnimationController::sway(ImageActor& a, float t, float amplitude, float frequency) {
    if (a.skeleton.size() < 2) return;
    a.skeleton[0].current = a.skeleton[0].bind;
    const float s = std::sin(t * frequency);
    a.skeleton[1].current = a.skeleton[1].bind + Vec2{0.0f, s * amplitude};
    a.skeleton[1].current_rotation = s * amplitude * 0.01f;
    ImagePipeline::update_bones(a);
}
void AnimationController::reset(ImageActor& a) {
    for (auto& j : a.skeleton) { j.current = j.bind; j.current_rotation = j.bind_rotation; }
    ImagePipeline::update_bones(a);
}
}
