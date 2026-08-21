#include "aethera/animation_controller.hpp"
#include "aethera/engine.hpp"
#include "aethera/image_actor_renderer.hpp"
#include "aethera/image_loader.hpp"
#include "aethera/image_pipeline.hpp"
#include <chrono>
#include <cmath>
#include <thread>

int main(int argc, char** argv) {
    if (argc < 2) {
        return 2;
    }

    aethera::ImageRgba8 image;
    if (!aethera::ImageLoader::load(argv[1], image)) {
        return 3;
    }

    aethera::Engine engine(960, 600, "Aethera Image Actor Demo");
    if (!engine.valid()) {
        return 4;
    }

    aethera::ImageActor actor = aethera::ImagePipeline::build(image);
    aethera::ImageActorRenderer actor_renderer(engine.renderer());

    using clock = std::chrono::steady_clock;
    auto previous = clock::now();
    float time = 0.0f;
    while (engine.pump_events()) {
        const auto now = clock::now();
        const float dt = std::chrono::duration<float>(now - previous).count();
        previous = now;
        time += dt;

        aethera::AnimationController::idle(actor, time);
        engine.update(dt);
        engine.render();
        actor_renderer.draw(actor, 220.0f, 90.0f);
        SDL_RenderPresent(engine.renderer());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}
