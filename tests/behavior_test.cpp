#include "aethera/behavior.hpp"

#include <cassert>

int main() {
    aethera::BehaviorSystem behavior;
    int calls = 0;

    behavior.add_rule({aethera::BehaviorEvent::Update, "walk",
                       [&](const aethera::BehaviorContext& context) {
                           assert(context.state == "walk");
                           ++calls;
                       }});

    behavior.emit(aethera::BehaviorEvent::Update, {0.016f, "idle", {}, 0.0f});
    assert(calls == 0);
    behavior.emit(aethera::BehaviorEvent::Update, {0.016f, "walk", {}, 1.0f});
    assert(calls == 1);

    behavior.set_flag("alert", true);
    assert(behavior.flag("alert"));
    assert(!behavior.flag("missing"));
    return 0;
}
