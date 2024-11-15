#include <Aether/Core/Application.hpp>
#include <Aether/Core/ServiceLocator/ServiceLocator.hpp>
#include <Aether/Core/Contexts/AppContext.hpp>
#include <Aether/Core/Contexts/VulkanContext.hpp>
#include <Aether/Core/Services/Clock.hpp>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>

#include <SDL3/SDL_vulkan.h>
#include <Aether/Core/Render/Shader.hpp>
#include <fstream>


namespace Aether {

    std::string loadBinary(const std::string& path) {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file.is_open()) {
                throw std::runtime_error("Error: Failed to open file: " + path);
            }
            std::streampos fileSize = file.tellg();
            std::string buffer;
            buffer.resize(fileSize);
            file.seekg(0);
            file.read(buffer.data(), buffer.size());
        return buffer;
    }

    Application& Application::getInstance() {
        static Application instance;
        return instance;
    }

    static void parseVkResult(VkResult err)
    {
        if (err == 0)
            return;
        throw std::runtime_error("Error: VkResult = " + std::to_string(err));;
        if (err < 0)
            abort();
    }

    void Application::Initialize( int argc, char** argv ) {
        spdlog::info("Initializing application...");

        auto *appContext = new AppContext{
            .state = {
                .isRunning = true,
                .isDebugging = false,
            },
            .windowProp = {
                .title = "Aether",
                .width = 1270,
                .height = 720,
                .isBorderless = false,
                .isHighPixelDensity = false,
                .isVsync = false,
            },
            .rendererProp = {
                    .clearColor = { 0.3f, 0.5f, 0.4f, 1.0f },
            }
        };

        ServiceLocator::Provide<AppContext>( appContext );

        auto vulkanContext = std::make_shared<Aether::VulkanContext>();
        ServiceLocator::Provide<VulkanContext>( vulkanContext );

        // Init SDL

        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            throw std::runtime_error("Error: SDL_Init(): " + std::string(SDL_GetError()));
        }

        if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
            throw std::runtime_error("Error: IMG_Init(): " + std::string(SDL_GetError()));
        }

        if (Mix_Init(MIX_INIT_OGG) == 0) {
            throw std::runtime_error("Error: Mix_Init(): " + std::string(SDL_GetError()));
        }

        spdlog::info("SDL initialized");

        {
            // Setup Audio
            SDL_AudioDeviceID deviceId = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;

            auto *spec = new SDL_AudioSpec();
            spec->channels = 2;
            spec->freq = 44100;
            spec->format = SDL_AUDIO_S16LE;
            if (!Mix_OpenAudio(deviceId, spec)) {
                spdlog::error("Error: Mix_OpenAudio(): {}", SDL_GetError());
                throw std::runtime_error("Error: Mix_OpenAudio(): " + std::string(SDL_GetError()));
            }
            delete spec;
        }

        {
            spdlog::info("Creating window...");
            // Set Window Properties
            SDL_PropertiesID props = SDL_CreateProperties();
            SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING,
                                  appContext->windowProp.title.c_str());
            SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, appContext->windowProp.width);
            SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, appContext->windowProp.height);
            SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN,
                                  appContext->windowProp.isBorderless); // Borderless
            SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true); // HDPI
            SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, true);

            // Init Window
            appContext->window = SDL_CreateWindowWithProperties(props);
            if (appContext->window == nullptr) {
                throw std::runtime_error("Error: SDL_CreateWindow(): " + std::string(SDL_GetError()));
            }
            SDL_DestroyProperties(props);
        }

        // Init Vulkan
        {

            vulkanContext->initialize();

            spdlog::debug("Instance: {}", static_cast<void*>(vulkanContext->instance));
            spdlog::debug("Device: {}", static_cast<void*>(vulkanContext->device));
            spdlog::debug("Physical Device: {}", static_cast<void*>(vulkanContext->phyDevice));
            spdlog::debug("Surface: {}", static_cast<void*>(vulkanContext->surface));
            spdlog::debug("Graphics Queue Family Index: {}", vulkanContext->queueFamilyIndices.graphics.value());
            spdlog::debug("Present Queue Family Index: {}", vulkanContext->queueFamilyIndices.present.value());

            std::shared_ptr<Shader> shader = std::make_shared<Shader>();

            // Load Shaders
            shader->initialize(
                    loadBinary("res/shaders/default.vert.spv"),
                    loadBinary("res/shaders/default.frag.spv"));
            ServiceLocator::Provide<Shader>( shader );

            vulkanContext->renderer->initialize();
            vulkanContext->renderProcess->initializeRenderPass();
            vulkanContext->renderProcess->initializeLayout();
            vulkanContext->swapchain->createFramebuffers(appContext->windowProp.width, appContext->windowProp.height);
            vulkanContext->renderProcess->initializePipeline(appContext->windowProp.width, appContext->windowProp.height);

        }

        {
            spdlog::info("Creating renderer...");
            // Init Renderer
            SDL_PropertiesID props = SDL_CreateProperties();
            SDL_SetStringProperty(props, SDL_PROP_RENDERER_CREATE_NAME_STRING, "Vulkan");
            SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER, appContext->window);
            SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER,
                                  appContext->windowProp.isVsync ? 1 : 0);
            SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_VULKAN_INSTANCE_POINTER, vulkanContext->instance);
//            SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_VULKAN_DEVICE_POINTER, vulkanContext->device);
//            SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_VULKAN_PHYSICAL_DEVICE_POINTER, vulkanContext->phyDevice);
//            SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_VULKAN_SURFACE_NUMBER, 1);
//            SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_VULKAN_GRAPHICS_QUEUE_FAMILY_INDEX_NUMBER, vulkanContext->queueFamilyIndices.graphics.value());
//            SDL_SetNumberProperty(props, SDL_PROP_RENDERER_CREATE_VULKAN_PRESENT_QUEUE_FAMILY_INDEX_NUMBER, vulkanContext->queueFamilyIndices.present.value());


            appContext->renderer = SDL_CreateRendererWithProperties(props);
            if (appContext->renderer == nullptr) {
                throw std::runtime_error("Error: SDL_CreateRenderer(): " + std::string(SDL_GetError()));
            }
            SDL_DestroyProperties(props);
        }

        {
            // Init Clock
            auto clock = std::make_shared<Clock>();
            clock->logic.target_fps = 20.0f;
            clock->render.target_fps = 60.0f;

            clock->logic.dt = 1000.0f / clock->logic.target_fps;
            clock->render.dt = 1000.0f / clock->render.target_fps;
            ServiceLocator::Provide<Clock>( clock );
        }

    }


    void Application::Process() {
        spdlog::info("Processing application...");

        auto appContext = ServiceLocator::Get<AppContext>();
        auto& renderer = ServiceLocator::Get<VulkanContext>()->renderer;

        while (appContext->state.isRunning) {
            auto clock = ServiceLocator::Get<Clock>();
            clock->render.t1 = std::chrono::steady_clock::now();

            {
                // Process Events
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT:
                            appContext->state.isRunning = false;
                            break;
                        case SDL_EVENT_KEY_DOWN:
                            if (event.key.key == SDLK_ESCAPE) {
                                appContext->state.isRunning = false;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

            renderer->DrawTrangle();

            auto TimeStampA = std::chrono::system_clock::now();
            std::chrono::duration<double, std::milli> work_time = clock->render.t1 - clock->render.t2;

            if (work_time.count() < clock->render.dt) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(clock->render.dt - work_time.count())));
            }
            clock->render.t2 = std::chrono::steady_clock::now();
        }
    }

    void Application::Terminate() {
        spdlog::info("Terminating application...");
        ServiceLocator::Get<VulkanContext>()->device.waitIdle();

        // Cleanup
        Mix_CloseAudio();

        SDL_DestroyWindow(ServiceLocator::Get<AppContext>()->window);
        SDL_DestroyRenderer(ServiceLocator::Get<AppContext>()->renderer);
        Mix_Quit();
        IMG_Quit();

        ServiceLocator::Get<Shader>()->terminate();
        ServiceLocator::Get<VulkanContext>()->terminate();
        ServiceLocator::Destroy();
        SDL_Quit();
    }

    Application::~Application() {
        Terminate();
    }
}