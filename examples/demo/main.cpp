#include "aethera/engine.hpp"
#include "aethera/mesh.hpp"
#include "aethera/mesh_renderer.hpp"

#include <SDL.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>

int main() {
    aethera::Engine engine(960, 600, "Aethera - Deformation Demo");
    if (!engine.valid()) return 1;

    std::vector<aethera::MeshVertex> mesh(4);
    mesh[0].position = {260, 170}; mesh[0].uv = {0, 0}; mesh[0].bone0 = 0; mesh[0].weight0 = 1;
    mesh[1].position = {500, 170}; mesh[1].uv = {1, 0}; mesh[1].bone0 = 1; mesh[1].weight0 = 1;
    mesh[2].position = {260, 430}; mesh[2].uv = {0, 1}; mesh[2].bone0 = 0; mesh[2].weight0 = 1;
    mesh[3].position = {500, 430}; mesh[3].uv = {1, 1}; mesh[3].bone0 = 1; mesh[3].weight0 = 1;
    std::vector<aethera::MeshBone> bones(2);
    bones[0].bind_position = {260, 300}; bones[0].current_position = bones[0].bind_position;
    bones[1].bind_position = {500, 300}; bones[1].current_position = bones[1].bind_position;
    const std::vector<std::uint32_t> indices = {0, 2, 1, 1, 2, 3};

    SDL_Renderer* renderer = engine.renderer();
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET, 240, 260);
    if (!texture) return 1;
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
    SDL_Rect rect{0, 0, 240, 260};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderTarget(renderer, nullptr);

    using clock = std::chrono::steady_clock;
    auto previous = clock::now();
    float time = 0.0f;
    while (engine.pump_events()) {
        const auto now = clock::now();
        const float dt = std::chrono::duration<float>(now - previous).count();
        previous = now;
        time += dt;

        bones[1].current_rotation = std::sin(time * 1.8f) * 0.35f;
        bones[1].current_position = {500.0f, 300.0f + std::sin(time * 1.8f) * 20.0f};

        engine.update(dt);
        engine.render();
        const auto draw = aethera::MeshRenderer::build(mesh, bones, indices);
        aethera::MeshRenderer::draw(renderer, texture, draw);
        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    SDL_DestroyTexture(texture);
    return 0;
}
