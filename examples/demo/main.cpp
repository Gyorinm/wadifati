#include "aethera/engine.hpp"

#include <chrono>
#include <thread>

int main() {
    aethera::Engine engine(960, 600, "Aethera Engine Demo");
    if (!engine.valid()) {
        return 1;
    }

    using clock = std::chrono::steady_clock;
    auto previous = clock::now();

    while (engine.pump_events()) {
        const auto now = clock::now();
        const float dt = std::chrono::duration<float>(now - previous).count();
        previous = now;

        engine.update(dt);
        engine.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
