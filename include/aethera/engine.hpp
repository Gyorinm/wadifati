#pragma once

#include <SDL.h>
#include <memory>

#include "aethera/animation_graph.hpp"
#include "aethera/behavior.hpp"
#include "aethera/image.hpp"
#include "aethera/image_object.hpp"
#include "aethera/image_renderer.hpp"
#include "aethera/object.hpp"
#include "aethera/physics.hpp"
#include "aethera/procedural_animation.hpp"
#include "aethera/renderer.hpp"

namespace aethera {

class Engine {
public:
    Engine(int width, int height, const char* title);
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    bool valid() const { return window_ != nullptr && renderer_ != nullptr; }
    bool pump_events();
    void update(float dt);
    void render();

    PhysicsWorld& physics() { return physics_; }
    LivingObject& world_object() { return object_; }
    ImageObject& image_object() { return image_object_; }
    AnimationGraph& animation() { return animation_; }
    BehaviorSystem& behavior() { return behavior_; }

private:
    SDL_Window* window_{};
    SDL_Renderer* renderer_{};
    std::unique_ptr<ImageRenderer> image_renderer_;

    PhysicsWorld physics_{};
    LivingObject object_{"demo"};
    ProceduralAnimator animator_{};
    AnimationGraph animation_{};
    BehaviorSystem behavior_{};

    ImageRgba8 demo_image_{};
    ImageObject image_object_{"demo_image"};

    float elapsed_{0.0f};
};

} // namespace aethera
