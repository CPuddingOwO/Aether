#pragma once

#include <Aether/Core/Vectorum.hpp>
#include <string>

namespace ae {
    typedef uint64_t Uint64;

    struct AppState {
        bool isRunning;
        bool isPaused;
        bool isDebugging;
        float deltaTime;
        bool openDebugOverlay;
    };

    struct WindowProperties {
        std::string title;
        int width;
        int height;
        bool isBorderless;
        bool isHighPixelDensity;
        bool isVsync;
    };

    struct RendererProperties {
        Vec4<float> clearColor;
    };

    struct Clock {
        float fps;
        float dt;
        Uint64 t1;
        Uint64 t2;
    };


    struct AppContext {
        AppState state{};
        WindowProperties windowProp;
        RendererProperties rendererProp;
        Clock clock{};
    };
}