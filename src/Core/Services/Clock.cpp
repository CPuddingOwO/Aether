#include <Aether/Core/Services/Clock.hpp>

namespace Aether {
    ClockOptions::ClockOptions() {
        t1 = std::chrono::steady_clock::now();
    }
}