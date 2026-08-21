#pragma once

#include <SDL.h>

#include "aethera/object.hpp"
#include "aethera/physics.hpp"
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

private:
    SDL_Window* window_{};
    SDL_Renderer* renderer_{};
    PhysicsWorld physics_{};
    LivingObject object_{"demo"};
    float elapsed_{0.0f};
};

} // namespace aethera
