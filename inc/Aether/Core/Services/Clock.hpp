#pragma once
#include <chrono>
#include <thread>

namespace Aether {

    struct ClockOptions final {
        ClockOptions();

        bool enabled = true;
        float target_fps = 60.0f;
        float dt = 0.0f;
        std::chrono::time_point<std::chrono::steady_clock> t1;
        std::chrono::time_point<std::chrono::steady_clock> t2;
    };

    class Clock final {
    public:
        ClockOptions logic;
        ClockOptions render;
    };

}