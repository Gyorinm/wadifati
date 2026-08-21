#include "aethera/animation_graph.hpp"
#include "aethera/behavior.hpp"
#include "aethera/image.hpp"
#include "aethera/image_object.hpp"
#include "aethera/physics.hpp"
#include "aethera/script.hpp"
#include "aethera/script_runtime.hpp"

#include <cassert>
#include <cmath>

int main() {
    const char* source = R"(
object fighter
when animate
    rotate_part arm 90
    move_part arm 5 -2
    reach 0 1 2 20 10
)";

    aethera::ScriptParser parser;
    aethera::ScriptProgram program;
    std::vector<aethera::ScriptError> errors;
    assert(parser.parse(source, program, errors));
    assert(errors.empty());

    aethera::ImageRgba8 image;
    image.width = 8;
    image.height = 8;
    image.pixels.assign(8 * 8 * 4, 255);

    aethera::ImageObject image_object("fighter");
    image_object.set_image(&image);
    aethera::ImageNode arm;
    arm.name = "arm";
    image_object.add_node(arm);

    aethera::PhysicsWorld physics;
    for (aethera::Vec2 p : {aethera::Vec2{0,0}, aethera::Vec2{10,0}, aethera::Vec2{20,0}}) {
        aethera::BodyPoint point;
        point.position = p;
        point.previous_position = p;
        point.pinned = true;
        point.inverse_mass = 0.0f;
        physics.add_point(point);
    }

    aethera::AnimationGraph animation;
    aethera::BehaviorSystem behavior;
    aethera::ScriptRuntime runtime;
    runtime.load(program);

    assert(runtime.run_trigger("animate", animation, behavior, physics, image_object));
    const auto* node = image_object.node(0);
    assert(node != nullptr);
    assert(std::fabs(node->local.rotation - 1.5707963f) < 1e-4f);
    assert(std::fabs(node->local.position.x - 5.0f) < 1e-5f);
    assert(std::fabs(node->local.position.y + 2.0f) < 1e-5f);
    return 0;
}
