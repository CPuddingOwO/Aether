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


    struct AppContext {
        AppState state{};
        WindowProperties windowProp;
        RendererProperties rendererProp;

        SDL_Window* window{};
        SDL_Renderer* renderer{};
    };
}