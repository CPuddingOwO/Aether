#pragma once

#include <Aether/Core/Vectorum.hpp>

#include <string>
#include <SDL3_image/SDL_image.h>

namespace Aether {

    struct AppState {
        bool isRunning;
        bool isPaused;
        bool isDebugging;
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
        float renderDt;
        float updateDt;
        Uint64 t1;
        Uint64 t2;
    };


    struct AppContext {
        AppState state{};
        WindowProperties windowProp;
        RendererProperties rendererProp;
        Clock clock{};

        SDL_Window* window{};
        SDL_Renderer* renderer{};
    };
}