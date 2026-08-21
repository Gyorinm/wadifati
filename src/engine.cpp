#include "aethera/engine.hpp"

#include <cmath>
#include <cstdlib>

namespace aethera {

namespace {

void make_demo(LivingObject& object, PhysicsWorld& physics) {
    const Vec2 origin{480.0f, 220.0f};
    const Vec2 local[] = {
        {0.0f, 120.0f},   // pelvis
        {0.0f, 40.0f},    // chest
        {0.0f, -35.0f},   // head
        {-55.0f, 55.0f},  // left elbow
        {-90.0f, 90.0f},  // left hand
        {55.0f, 55.0f},   // right elbow
        {90.0f, 90.0f},   // right hand
        {-35.0f, 190.0f}, // left knee
        {-45.0f, 285.0f}, // left foot
        {35.0f, 190.0f},  // right knee
        {45.0f, 285.0f}   // right foot
    };

    const float masses[] = {0.0f, 1.0f, 1.0f, 1.0f, 0.7f, 1.0f, 0.7f, 1.0f, 0.7f, 1.0f, 0.7f};

    for (int i = 0; i < 11; ++i) {
        BodyPoint point;
        point.position = origin + local[i];
        point.previous_position = point.position;
        point.inverse_mass = masses[i] == 0.0f ? 0.0f : 1.0f / masses[i];
        point.pinned = (i == 0);
        physics.add_point(point);
    }

    auto bone = [&](std::size_t a, std::size_t b) {
        const float rest = length(physics.points()[b].position - physics.points()[a].position);
        physics.add_constraint({a, b, rest, 0.95f});
        object.add_bone(a, b);
    };

    bone(0, 1);
    bone(1, 2);
    bone(1, 3);
    bone(3, 4);
    bone(1, 5);
    bone(5, 6);
    bone(0, 7);
    bone(7, 8);
    bone(0, 9);
    bone(9, 10);

    object.add_part({"pelvis", PartKind::Point, 0, 9.0f, 5.0f});
    object.add_part({"chest", PartKind::Point, 1, 11.0f, 6.0f});
    object.add_part({"head", PartKind::Point, 2, 18.0f, 6.0f});
    object.add_part({"left_hand", PartKind::Point, 4, 7.0f, 5.0f});
    object.add_part({"right_hand", PartKind::Point, 6, 7.0f, 5.0f});
    object.add_part({"left_foot", PartKind::Point, 8, 8.0f, 5.0f});
    object.add_part({"right_foot", PartKind::Point, 10, 8.0f, 5.0f});
}

} // namespace

Engine::Engine(int width, int height, const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        return;
    }

    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_SHOWN);
    if (window_ == nullptr) {
        SDL_Quit();
        return;
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_ == nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return;
    }

    make_demo(object_, physics_);
}

Engine::~Engine() {
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

bool Engine::pump_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            return false;
        }
    }
    return true;
}

void Engine::update(float dt) {
    elapsed_ += dt;

    // The pinned root is our first procedural animation anchor.
    // A future rig will replace this with an explicit animation graph.
    auto& root = physics_.points()[0];
    const float base_y = 340.0f;
    root.position.y = base_y + std::sin(elapsed_ * 2.2f) * 2.5f;
    root.previous_position = root.position;

    physics_.step(dt);
}

void Engine::render() {
    Renderer renderer(renderer_);
    renderer.begin();
    renderer.living_object(object_, physics_);
    renderer.end();
}

} // namespace aethera
