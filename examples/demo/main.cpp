#include "aethera/engine.hpp"
#include "aethera/image.hpp"
#include "aethera/mesh.hpp"
#include "aethera/mesh_renderer.hpp"
#include "aethera/shape_mesh.hpp"
#include "aethera/skeleton_pose.hpp"
#include "aethera/texture_loader.hpp"
#include <SDL.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    aethera::Engine engine(960, 600, "Aethera - Shape Mesh Demo");
    if (!engine.valid()) return 1;
    SDL_Renderer* renderer = engine.renderer();

    SDL_Texture* texture = nullptr;
    int tw = 280, th = 360;
    if (argc > 1) texture = aethera::TextureLoader::load(renderer, argv[1]);
    if (texture) SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);
    else {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tw, th);
        if (!texture) return 1;
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 0); SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 90, 170, 230, 255);
        SDL_Rect body{30, 20, tw - 60, th - 40}; SDL_RenderFillRect(renderer, &body);
        SDL_SetRenderTarget(renderer, nullptr);
    }

    aethera::ImageRgba8 image;
    image.width = tw; image.height = th;
    image.pixels.resize(static_cast<std::size_t>(tw * th * 4), 255);
    auto mesh = aethera::ShapeMeshGenerator::vertices_from_alpha(image, 10, 14, 8);
    auto indices = aethera::ShapeMeshGenerator::indices(10, 14);

    std::vector<aethera::SkeletonJoint> joints(2);
    joints[0].bind = {0, th * 0.5f}; joints[0].current = joints[0].bind;
    joints[1].parent = 0; joints[1].bind = {static_cast<float>(tw), th * 0.5f}; joints[1].current = joints[1].bind;

    const float ox = 220.0f, oy = 120.0f;
    using clock = std::chrono::steady_clock;
    auto previous = clock::now(); float time = 0.0f;
    while (engine.pump_events()) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - previous).count(); previous = now; time += dt;
        joints[1].current = joints[1].bind;
        joints[0].current_rotation = std::sin(time * 1.6f) * 0.08f;
        joints[1].current_rotation = std::sin(time * 1.6f) * 0.35f;
        aethera::SkeletonPose::propagate(joints);
        auto bones = aethera::SkeletonPose::to_mesh_bones(joints);
        for (auto& v : mesh) {
            const float u = v.uv.x;
            v.bone0 = u < 0.5f ? 0u : 1u; v.bone1 = v.bone0;
            v.weight0 = 1.0f; v.weight1 = 0.0f;
            if (u > 0.4f && u < 0.6f) { v.bone0 = 0; v.bone1 = 1; v.weight0 = 0.5f; v.weight1 = 0.5f; }
        }
        engine.update(dt); engine.render();
        const auto draw = aethera::MeshRenderer::build(mesh, bones, indices, {ox, oy});
        aethera::MeshRenderer::draw(renderer, texture, draw);
        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    SDL_DestroyTexture(texture);
    return 0;
}
