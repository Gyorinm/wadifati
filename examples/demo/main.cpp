#include "aethera/engine.hpp"
#include "aethera/image_loader.hpp"
#include "aethera/image_pipeline.hpp"
#include "aethera/animation_controller.hpp"
#include "aethera/mesh_renderer.hpp"
#include "aethera/texture_loader.hpp"
#include <SDL.h>
#include <chrono>
#include <thread>
#include <vector>

int main(int argc, char** argv) {
    aethera::Engine engine(960, 600, "Aethera - Image Actor MVP");
    if (!engine.valid()) return 1;
    SDL_Renderer* renderer = engine.renderer();

    aethera::ImageRgba8 image;
    if (argc > 1) aethera::ImageLoader::load(argv[1], image);
    if (!image.valid()) {
        image.width = 280; image.height = 360;
        image.pixels.resize(static_cast<std::size_t>(image.width * image.height * 4), 255);
    }

    SDL_Texture* texture = argc > 1 ? aethera::TextureLoader::load(renderer, argv[1]) : nullptr;
    if (!texture) {
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, image.width, image.height);
        if (!texture) return 1;
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, 60, 100, 150, 255); SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, nullptr);
    }

    auto actor = aethera::ImagePipeline::build(image, 12, 16);
    const aethera::Vec2 offset{240.0f, 120.0f};
    using clock = std::chrono::steady_clock;
    auto previous = clock::now(); float time = 0.0f;

    while (engine.pump_events()) {
        const auto now = clock::now();
        const float dt = std::chrono::duration<float>(now - previous).count();
        previous = now; time += dt;
        aethera::AnimationController::idle(actor, time);
        engine.update(dt);
        engine.render();
        const auto draw = aethera::MeshRenderer::build(actor.mesh, actor.bones, actor.indices, offset);
        aethera::MeshRenderer::draw(renderer, texture, draw);
        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    SDL_DestroyTexture(texture);
    return 0;
}
