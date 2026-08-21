#pragma once

#include <SDL.h>

#include "aethera/math.hpp"
#include "aethera/object.hpp"
#include "aethera/physics.hpp"
#include "aethera/image_reconstruction.hpp"

namespace aethera {

class MeshTextureRenderer;

class Renderer {
public:
    explicit Renderer(SDL_Renderer* renderer);

    void begin();
    void end();
    void line(Vec2 a, Vec2 b, float width = 2.0f);
    void circle(Vec2 center, float radius);
    void living_object(const LivingObject& object, const PhysicsWorld& physics);
    void reconstructed_image(const ImageReconstructionResult& result);

private:
    SDL_Renderer* renderer_{};
    MeshTextureRenderer* mesh_renderer_{};
};

} // namespace aethera
