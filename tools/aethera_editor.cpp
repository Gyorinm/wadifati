#include "aethera/engine.hpp"
#include "aethera/editor.hpp"
#include "aethera/image_loader.hpp"
#include "aethera/image_actor_renderer.hpp"
#include "aethera/animation_controller.hpp"
#include "aethera/image_pipeline.hpp"
#include <SDL.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: aethera_editor <image>\n";
        return 2;
    }
    aethera::ImageRgba8 image;
    if (!aethera::ImageLoader::load(argv[1], image)) {
        std::cerr << "failed to load image: " << argv[1] << "\n";
        return 1;
    }
    aethera::Editor editor;
    editor.set_actor(aethera::ImagePipeline::build(image));
    aethera::Engine engine(1000, 700, "Aethera Editor");
    if (!engine.valid()) return 1;
    aethera::ImageActorRenderer renderer(engine.renderer());
    bool playing = true;
    bool running = true;
    float time = 0.0f;
    while (running) {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_1: editor.select_joint(0); break;
                    case SDLK_2: editor.select_joint(1); break;
                    case SDLK_LEFT: editor.move_selected({-5.0f, 0.0f}); break;
                    case SDLK_RIGHT: editor.move_selected({5.0f, 0.0f}); break;
                    case SDLK_UP: editor.move_selected({0.0f, -5.0f}); break;
                    case SDLK_DOWN: editor.move_selected({0.0f, 5.0f}); break;
                    case SDLK_Q: editor.rotate_selected(-0.08f); break;
                    case SDLK_E: editor.rotate_selected(0.08f); break;
                    case SDLK_SPACE: playing = !playing; break;
                    case SDLK_S: editor.save("aethera_scene.aethera"); break;
                    default: break;
                }
            }
        }
        time += 1.0f / 60.0f;
        if (playing && editor.actor()) aethera::AnimationController::idle(*editor.actor(), time);
        engine.update(1.0f / 60.0f);
        engine.render();
        if (editor.actor()) renderer.draw(*editor.actor(), 180.0f, 120.0f);
        SDL_RenderPresent(engine.renderer());
    }
    return 0;
}
