#include "aethera/engine.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace aethera {

namespace {

void make_demo(LivingObject& object, PhysicsWorld& physics) {
    const Vec2 origin{480.0f, 220.0f};
    const Vec2 local[] = {
        {0.0f, 120.0f}, {0.0f, 40.0f}, {0.0f, -35.0f}, {-55.0f, 55.0f}, {-90.0f, 90.0f},
        {55.0f, 55.0f}, {90.0f, 90.0f}, {-35.0f, 190.0f}, {-45.0f, 285.0f},
        {35.0f, 190.0f}, {45.0f, 285.0f}
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
    bone(0, 1); bone(1, 2); bone(1, 3); bone(3, 4); bone(1, 5); bone(5, 6);
    bone(0, 7); bone(7, 8); bone(0, 9); bone(9, 10);

    object.add_part({"pelvis", PartKind::Point, 0, 9.0f, 5.0f});
    object.add_part({"chest", PartKind::Point, 1, 11.0f, 6.0f});
    object.add_part({"head", PartKind::Point, 2, 18.0f, 6.0f});
    object.add_part({"left_hand", PartKind::Point, 4, 7.0f, 5.0f});
    object.add_part({"right_hand", PartKind::Point, 6, 7.0f, 5.0f});
    object.add_part({"left_foot", PartKind::Point, 8, 8.0f, 5.0f});
    object.add_part({"right_foot", PartKind::Point, 10, 8.0f, 5.0f});
}

void put_pixel(ImageRgba8& image, int x, int y, std::uint8_t r, std::uint8_t g,
               std::uint8_t b, std::uint8_t a = 255) {
    if (x < 0 || y < 0 || x >= image.width || y >= image.height) return;
    const std::size_t index = static_cast<std::size_t>((y * image.width + x) * 4);
    image.pixels[index + 0] = r;
    image.pixels[index + 1] = g;
    image.pixels[index + 2] = b;
    image.pixels[index + 3] = a;
}

void fill_rect(ImageRgba8& image, int x, int y, int w, int h,
               std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    for (int yy = y; yy < y + h; ++yy)
        for (int xx = x; xx < x + w; ++xx)
            put_pixel(image, xx, yy, r, g, b);
}

void fill_circle(ImageRgba8& image, int cx, int cy, int radius,
                 std::uint8_t r, std::uint8_t g, std::uint8_t b) {
    const int radius_sq = radius * radius;
    for (int y = cy - radius; y <= cy + radius; ++y) {
        for (int x = cx - radius; x <= cx + radius; ++x) {
            const int dx = x - cx;
            const int dy = y - cy;
            if (dx * dx + dy * dy <= radius_sq) put_pixel(image, x, y, r, g, b);
        }
    }
}

void make_demo_image(ImageRgba8& image) {
    image.width = 256;
    image.height = 128;
    image.pixels.assign(static_cast<std::size_t>(image.width * image.height * 4), 0);
    fill_circle(image, 32, 32, 22, 235, 205, 170);
    fill_rect(image, 72, 12, 48, 80, 55, 110, 195);
    fill_rect(image, 128, 20, 64, 18, 220, 90, 80);
    fill_rect(image, 128, 50, 64, 18, 220, 90, 80);
    fill_rect(image, 200, 8, 24, 80, 80, 200, 120);
    fill_rect(image, 228, 8, 24, 80, 80, 200, 120);
}

void make_image_object(ImageObject& object, const ImageRgba8& image) {
    object.set_image(&image);
    ImageNode root;
    root.name = "character_root";
    root.visual.name = "atlas_root";
    root.visual.source = Rect{0.0f, 0.0f, 256.0f, 128.0f};
    root.visual.pivot = {0.5f, 0.5f};
    root.local.position = {480.0f, 300.0f};
    root.local.scale = {0.75f, 0.75f};
    const std::size_t root_id = object.add_node(root);

    auto add_part = [&](const char* name, Rect source, Vec2 pivot, Vec2 pos) {
        ImageNode node;
        node.name = name;
        node.parent = root_id;
        node.visual.name = name;
        node.visual.source = source;
        node.visual.pivot = pivot;
        node.local.position = pos;
        object.add_node(node);
    };

    add_part("head", Rect{0, 0, 64, 64}, {0.5f, 0.5f}, {-90, -45});
    add_part("torso", Rect{64, 0, 64, 96}, {0.5f, 0.5f}, {-30, 5});
    add_part("arm_a", Rect{128, 16, 64, 24}, {0.05f, 0.5f}, {22, -5});
    add_part("arm_b", Rect{128, 48, 64, 24}, {0.05f, 0.5f}, {22, 28});
    add_part("leg_a", Rect{192, 0, 32, 96}, {0.5f, 0.1f}, {-18, 52});
    add_part("leg_b", Rect{224, 0, 32, 96}, {0.5f, 0.1f}, {10, 52});
    object.update_world_transforms();
}

} // namespace

Engine::Engine(int width, int height, const char* title) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) return;
    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               width, height, SDL_WINDOW_SHOWN);
    if (window_ == nullptr) { SDL_Quit(); return; }
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer_ == nullptr) { SDL_DestroyWindow(window_); window_ = nullptr; SDL_Quit(); return; }
    image_renderer_ = std::make_unique<ImageRenderer>(renderer_);

    animation_.add_transition({AnimationState::Idle, AnimationState::Walk, "move", 0.15f});
    animation_.add_transition({AnimationState::Walk, AnimationState::Idle, "stop", 0.15f});
    animation_.add_transition({AnimationState::Walk, AnimationState::Run, "sprint", 0.12f});
    animation_.add_transition({AnimationState::Run, AnimationState::Walk, "slow", 0.12f});
    animation_.add_transition({AnimationState::Idle, AnimationState::Jump, "jump", 0.08f});
    animation_.add_transition({AnimationState::Walk, AnimationState::Jump, "jump", 0.08f});
    animation_.add_transition({AnimationState::Run, AnimationState::Jump, "jump", 0.08f});
    animation_.add_transition({AnimationState::Jump, AnimationState::Fall, "airborne", 0.06f});
    animation_.add_transition({AnimationState::Fall, AnimationState::Land, "grounded", 0.06f});
    animation_.add_transition({AnimationState::Land, AnimationState::Idle, "recover", 0.10f});

    behavior_.add_rule({BehaviorEvent::EnterState, "walk", [](const BehaviorContext&) {}});
    behavior_.add_rule({BehaviorEvent::EnterState, "run", [](const BehaviorContext&) {}});
    behavior_.emit(BehaviorEvent::Spawn, {});

    animator_.set_state(MotionState::Walk);
    make_demo(object_, physics_);
    make_demo_image(demo_image_);
    make_image_object(image_object_, demo_image_);
}

Engine::~Engine() {
    image_renderer_.reset();
    if (renderer_ != nullptr) SDL_DestroyRenderer(renderer_);
    if (window_ != nullptr) SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Engine::pump_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return false;
    }
    return true;
}

void Engine::update(float dt) {
    elapsed_ += dt;
    animation_.update(dt);
    behavior_.emit(BehaviorEvent::Update, {dt, animation_state_name(animation_.state()), {}, animation_.state_time()});

    auto& root = physics_.points()[0];
    root.position.y = 340.0f + std::sin(elapsed_ * 2.2f) * 2.5f;
    root.previous_position = root.position;
    physics_.step(dt);
    animator_.update(dt);
    animator_.apply_walk_cycle(physics_, 8, 10);

    if (image_object_.nodes().size() >= 7) {
        auto& root_node = image_object_.nodes()[0];
        auto& head = image_object_.nodes()[1];
        auto& arm_a = image_object_.nodes()[3];
        auto& arm_b = image_object_.nodes()[4];
        auto& leg_a = image_object_.nodes()[5];
        auto& leg_b = image_object_.nodes()[6];

        const float gait = animation_.state() == AnimationState::Run ? 2.0f : 1.0f;
        root_node.local.position.y = 300.0f + std::sin(elapsed_ * 2.0f * gait) * (6.0f * gait);
        root_node.local.rotation = std::sin(elapsed_ * 0.8f) * 0.025f;
        head.local.rotation = std::sin(elapsed_ * 2.0f * gait) * 0.05f;
        arm_a.local.rotation = std::sin(elapsed_ * 3.5f * gait) * 0.35f;
        arm_b.local.rotation = std::sin(elapsed_ * 3.5f * gait + 1.57f) * 0.35f;
        leg_a.local.rotation = std::sin(elapsed_ * 2.5f * gait) * 0.18f;
        leg_b.local.rotation = -std::sin(elapsed_ * 2.5f * gait) * 0.18f;
        image_object_.update_world_transforms();
    }
}

void Engine::render() {
    Renderer renderer(renderer_);
    renderer.begin();
    renderer.living_object(object_, physics_);
    image_renderer_->draw(image_object_);
    renderer.end();
}

} // namespace aethera
