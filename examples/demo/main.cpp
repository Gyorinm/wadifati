#include "aethera/engine.hpp"
#include "aethera/mesh.hpp"
#include "aethera/mesh_renderer.hpp"
#include "aethera/texture_loader.hpp"
#include <SDL.h>
#include <chrono>
#include <cmath>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    aethera::Engine engine(960, 600, "Aethera - Image Deformation Demo");
    if (!engine.valid()) return 1;
    SDL_Renderer* renderer = engine.renderer();
    SDL_Texture* texture = nullptr;
    if (argc > 1) texture = aethera::TextureLoader::load(renderer, argv[1]);
    if (!texture) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 240, 260);
        if (!texture) return 1;
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255); SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, nullptr);
    }
    int tw = 240, th = 260;
    SDL_QueryTexture(texture, nullptr, nullptr, &tw, &th);
    const float x0 = 260.0f, y0 = 170.0f;
    std::vector<aethera::MeshVertex> mesh(4);
    mesh[0].position={x0,y0}; mesh[0].uv={0,0}; mesh[0].bone0=0; mesh[0].weight0=1;
    mesh[1].position={x0+tw,y0}; mesh[1].uv={1,0}; mesh[1].bone0=1; mesh[1].weight0=1;
    mesh[2].position={x0,y0+th}; mesh[2].uv={0,1}; mesh[2].bone0=0; mesh[2].weight0=1;
    mesh[3].position={x0+tw,y0+th}; mesh[3].uv={1,1}; mesh[3].bone0=1; mesh[3].weight0=1;
    std::vector<aethera::MeshBone> bones(2);
    bones[0].bind_position={x0,y0+th*0.5f}; bones[0].current_position=bones[0].bind_position;
    bones[1].bind_position={x0+tw,y0+th*0.5f}; bones[1].current_position=bones[1].bind_position;
    const std::vector<std::uint32_t> indices={0,2,1,1,2,3};
    using clock=std::chrono::steady_clock; auto previous=clock::now(); float time=0;
    while(engine.pump_events()) {
        auto now=clock::now(); float dt=std::chrono::duration<float>(now-previous).count(); previous=now; time+=dt;
        bones[1].current_rotation=std::sin(time*1.8f)*0.35f;
        bones[1].current_position={x0+tw,y0+th*0.5f+std::sin(time*1.8f)*20.0f};
        engine.update(dt); engine.render();
        const auto draw=aethera::MeshRenderer::build(mesh,bones,indices);
        aethera::MeshRenderer::draw(renderer,texture,draw); SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    SDL_DestroyTexture(texture); return 0;
}
