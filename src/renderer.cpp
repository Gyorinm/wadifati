#include "aethera/renderer.hpp"
#include "aethera/mesh_renderer.hpp"
#include "aethera/mesh_texture_renderer.hpp"

#include <cmath>
#include <memory>

namespace aethera {

Renderer::Renderer(SDL_Renderer* renderer) : renderer_(renderer), mesh_renderer_(std::make_unique<MeshTextureRenderer>(renderer)) {}

void Renderer::begin() {
    SDL_SetRenderDrawColor(renderer_, 14, 16, 22, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::end() { SDL_RenderPresent(renderer_); }

void Renderer::line(Vec2 a, Vec2 b, float) {
    SDL_SetRenderDrawColor(renderer_, 220, 225, 235, 255);
    SDL_RenderDrawLine(renderer_, static_cast<int>(std::lround(a.x)), static_cast<int>(std::lround(a.y)),
                       static_cast<int>(std::lround(b.x)), static_cast<int>(std::lround(b.y)));
}

void Renderer::circle(Vec2 center, float radius) {
    SDL_SetRenderDrawColor(renderer_, 240, 240, 245, 255);
    const int cx = static_cast<int>(std::lround(center.x));
    const int cy = static_cast<int>(std::lround(center.y));
    const int r = static_cast<int>(std::lround(radius));
    for (int y = -r; y <= r; ++y) {
        const int x = static_cast<int>(std::sqrt(static_cast<float>(r * r - y * y)));
        SDL_RenderDrawLine(renderer_, cx - x, cy + y, cx + x, cy + y);
    }
}

void Renderer::living_object(const LivingObject& object, const PhysicsWorld& physics) {
    const auto& points = physics.points();
    for (const auto& [a, b] : object.bones()) if (a < points.size() && b < points.size()) line(points[a].position, points[b].position, 5.0f);
    for (const auto& part : object.parts()) if (part.physics_point < points.size()) circle(points[part.physics_point].position, part.radius);
}

void Renderer::reconstructed_image(const ImageReconstructionResult& result) {
    if (!mesh_renderer_) return;
    const std::size_t count = std::min(result.parts.size(), result.object.nodes().size());
    MeshRenderer bridge;
    for (std::size_t i = 0; i < count; ++i) {
        const auto& part = result.parts[i];
        const auto& node = result.object.nodes()[i];
        if (!node.visible || !part.layer.valid()) continue;
        const MeshDrawData draw_data = bridge.build_draw_data(part.mesh_vertices, part.mesh_vertices, part.mesh_indices);
        mesh_renderer_->draw(part.layer, draw_data, node.local.position);
    }
}

} // namespace aethera
