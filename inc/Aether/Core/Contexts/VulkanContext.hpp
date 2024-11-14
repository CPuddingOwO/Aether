#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>
#include <Aether/Core/Services/Swapchain.hpp>
#include <Aether/Core/Render/Renderer.hpp>
#include <Aether/Core/Render/RenderProcess.hpp>

namespace Aether {
    class VulkanContext {
    public:
        VulkanContext() = default;
        ~VulkanContext() = default;

        struct QueueFamilyIndices final {
            std::optional<uint32_t> graphics = std::nullopt;
            std::optional<uint32_t> present  = std::nullopt;

            [[nodiscard]] bool isComplete() const { return graphics.has_value() && present.has_value(); }
        };

        struct Extensions final {
            uint32_t extensionCount;
            std::vector<const char*> extensions;
        };

        vk::AllocationCallbacks     *allocCallbacks         = nullptr;
        vk::SurfaceKHR              surface                 = VK_NULL_HANDLE;
        vk::Instance                instance                = VK_NULL_HANDLE;
        vk::PhysicalDevice          phyDevice               = VK_NULL_HANDLE;
        vk::Device                  device                  = VK_NULL_HANDLE;
        vk::Queue                   graphicsQueue           = VK_NULL_HANDLE;
        vk::Queue                   presentQueue            = VK_NULL_HANDLE;
        QueueFamilyIndices          queueFamilyIndices;
        Extensions                  instanceExtensions;
        Extensions                  deviceExtensions;

        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderProcess> renderProcess;
        std::unique_ptr<Renderer> renderer;


        void initialize();
        void terminate();

    private:
        void createInstance();
        void pickupPhysicalDevice();
        void createDevice();
        void queryQueueFamilyIndex();
        void getQueues();
        void setDeviceExtensions();
        void setInstanceExtensions();
        void createSurface();
    };
}