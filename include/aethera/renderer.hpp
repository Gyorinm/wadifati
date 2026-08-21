#pragma once

#include <SDL.h>

#include "aethera/math.hpp"
#include "aethera/object.hpp"
#include "aethera/physics.hpp"

namespace aethera {

class Renderer {
public:
    explicit Renderer(SDL_Renderer* renderer) : renderer_(renderer) {}

    void begin();
    void end();
    void line(Vec2 a, Vec2 b, float width = 2.0f);
    void circle(Vec2 center, float radius);
    void living_object(const LivingObject& object, const PhysicsWorld& physics);

private:
    SDL_Renderer* renderer_{};
};

} // namespace aethera
