#include <Aether/Core/Contexts/VulkanContext.hpp>
#include <spdlog/spdlog.h>
#include <SDL3/SDL_vulkan.h>
#include <Aether/Core/Contexts/AppContext.hpp>
#include <Aether/Core/ServiceLocator/ServiceLocator.hpp>
#include <memory>

namespace Aether {

    // Public functions
    void VulkanContext::initialize() {
        setInstanceExtensions();
        setDeviceExtensions();
        createInstance();
        createSurface();
        pickupPhysicalDevice();
        queryQueueFamilyIndex();
        createDevice();
        getQueues();
        swapchain = std::make_unique<Swapchain>(ServiceLocator::Get<AppContext>()->windowProp.width, ServiceLocator::Get<AppContext>()->windowProp.height);
        renderProcess = std::make_unique<RenderProcess>();
        renderer = std::make_unique<Renderer>();
    }
    void VulkanContext::terminate() {
        renderer.reset();
        renderProcess.reset();
        swapchain.reset();
        instance.destroySurfaceKHR(surface);
        device.destroy();
        instance.destroy();
    }

    void VulkanContext::createInstance() {
        vk::ApplicationInfo appInfo = {};
        appInfo.setApiVersion(VK_API_VERSION_1_0)
               .setPApplicationName("Aether")
               .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));

        vk::InstanceCreateInfo instanceInfo = {};
        instanceInfo.setPApplicationInfo(&appInfo)
                    .setEnabledExtensionCount(instanceExtensions.extensions.size())
                    .setPpEnabledExtensionNames(instanceExtensions.extensions.data());


        std::vector<const char*> enabledLayers = { };
        uint32_t layerCount = 0;
        auto layers = vk::enumerateInstanceLayerProperties();
        spdlog::debug("[Vulkan] Available Layers({}): ", layers.size());
        for (const auto& layer : layers) {
            spdlog::debug("[Vulkan] \tLayer: {}", std::string(layer.layerName));
        }
        instanceInfo.setEnabledLayerCount(layerCount);
        instanceInfo.setPpEnabledLayerNames(enabledLayers.data());

        spdlog::debug("[Vulkan] Enabled Instance Extension: {}", instanceExtensions.extensionCount);
        for (const auto& extension : instanceExtensions.extensions) {
            spdlog::debug("[Vulkan] \tExtension: {}", std::string(extension));
        }

        vk::Result rst = vk::createInstance(&instanceInfo, allocCallbacks, &instance);

        if (rst != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create Vulkan instance");
        }
    }

    void VulkanContext::pickupPhysicalDevice() {
        uint32_t phyDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &phyDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> phyDevices(phyDeviceCount);
        vkEnumeratePhysicalDevices(instance, &phyDeviceCount, phyDevices.data());

        spdlog::debug("[Vulkan] Physical Devices({}): ", phyDeviceCount);
        for (const auto _phyDevice : phyDevices) {
            phyDevice = _phyDevice;
            spdlog::debug("[Vulkan] \tPhysical Device: {}", std::string(phyDevice.getProperties().deviceName));
        }
        phyDevice = phyDevices[2];
        spdlog::debug("[Vulkan] Pick Physical Device: {}", std::string(phyDevice.getProperties().deviceName));
    }

    void VulkanContext::createDevice() {
        std::vector<vk::DeviceQueueCreateInfo> queueInfos;
        float queuePriority = 1.0f;
        {
            vk::DeviceQueueCreateInfo queueInfo = {};
            queueInfo.setPQueuePriorities(&queuePriority);
            queueInfo.setQueueFamilyIndex(queueFamilyIndices.graphics.value());
            queueInfo.setQueueCount(1);
            queueInfos.push_back(queueInfo);
        }

        if (queueFamilyIndices.graphics.value() != queueFamilyIndices.present.value()) {
            vk::DeviceQueueCreateInfo queueInfo = {};
            queueInfo.setPQueuePriorities(&queuePriority);
            queueInfo.setQueueFamilyIndex(queueFamilyIndices.present.value());
            queueInfo.setQueueCount(1);
            queueInfos.push_back(queueInfo);
        }

        vk::PhysicalDeviceSynchronization2FeaturesKHR sync2Features{};
        sync2Features.synchronization2 = VK_TRUE;

        vk::DeviceCreateInfo deviceInfo = {};
        deviceInfo.setQueueCreateInfoCount(queueInfos.size())
                  .setPNext(&sync2Features)
                  .setQueueCreateInfos(queueInfos)
                  .setEnabledExtensionCount(deviceExtensions.extensionCount)
                  .setPpEnabledExtensionNames(deviceExtensions.extensions.data());

        spdlog::debug("[Vulkan] Enabled Device Extension: {}", deviceExtensions.extensionCount);
        for (const auto& extension : deviceExtensions.extensions) {
            spdlog::debug("[Vulkan] \tExtension: {}", std::string(extension));
        }

        device = phyDevice.createDevice(deviceInfo);

    }

    void  VulkanContext::queryQueueFamilyIndex() {
        auto properties = phyDevice.getQueueFamilyProperties();
        for (uint32_t i = 0; i < properties.size(); i++) {
            if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                queueFamilyIndices.graphics = i;
            }
            if (phyDevice.getSurfaceSupportKHR(i, surface)) {
                queueFamilyIndices.present = i;
            }
            if (queueFamilyIndices.isComplete()) {
                break;
            }
        }
    }

    void VulkanContext::getQueues() {
        graphicsQueue = device.getQueue(queueFamilyIndices.graphics.value(), 0);
        presentQueue = device.getQueue(queueFamilyIndices.present.value(), 0);
    }

    void VulkanContext::setInstanceExtensions() {
        auto exs = SDL_Vulkan_GetInstanceExtensions(&instanceExtensions.extensionCount);

        spdlog::debug("[Vulkan] Available Extensions({}): ", instanceExtensions.extensionCount);
        for (uint32_t i = 0; i < instanceExtensions.extensionCount; i++) {
            spdlog::debug("[Vulkan] \tExtension: {}", std::string(exs[i]));
            instanceExtensions.extensions.push_back(exs[i]);
        }

        instanceExtensions.extensionCount = instanceExtensions.extensions.size();
    }

    void VulkanContext::setDeviceExtensions() {
        deviceExtensions.extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceExtensions.extensions.push_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

        deviceExtensions.extensionCount = deviceExtensions.extensions.size();
    }

    void VulkanContext::createSurface() {
        VkSurfaceKHR cSurface = VK_NULL_HANDLE;
        auto appContext = ServiceLocator::Get<AppContext>();
        if ( !SDL_Vulkan_CreateSurface( appContext->window, instance, (VkAllocationCallbacks *)allocCallbacks, &cSurface) ) {
            throw std::runtime_error("Error: SDL_Vulkan_CreateSurface(): " + std::string(SDL_GetError()));
        }
        surface = vk::SurfaceKHR(cSurface);
    }
}