#include <Aether/Core/Application.hpp>
#include <Aether/Core/ServiceLocator/ServiceLocator.hpp>
#include <Aether/Core/Contexts/AppContext.hpp>


#include <iostream>

namespace ae {
    Application& Application::getInstance() {
        static Application instance;
        return instance;
    }

    void Application::Initialize( int argc, char** argv ) {
        std::cout << "Initializing application..." << std::endl;

        auto *appContext = new AppContext{
            .state = {
                .isRunning = true,
                .isDebugging = false,
            },
            .windowProp = {
                .title = "Aether",
                .width = 800,
                .height = 600,
                .isBorderless = false,
                .isHighPixelDensity = false,
                .isVsync = false,
            },
            .rendererProp = {
                    .clearColor = { 0.0f, 0.0f, 0.0f, 1.0f },
            },
            .clock = {
                    .fps = 60.0f,
                    .dt = 1000.f / 60.0f,
                    },
        };

        ServiceLocator::Provide<AppContext>( appContext );

    }


    void Application::Process() {
        std::cout << "Processing application..." << std::endl;
        auto appContext = ServiceLocator::Get<AppContext>();

            while (appContext->state.isRunning) {

            }
        // 处理逻辑
    }

    void Application::Terminate() {
        std::cout << "Terminating down application..." << std::endl;
        // 清理逻辑
    }

    Application::~Application() {
        // 资源释放
    }
}