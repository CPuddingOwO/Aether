#include "Aether/Core/Services/Swapchain.hpp"
#include "Aether/Core/Contexts/VulkanContext.hpp"
#include "Aether/Core/ServiceLocator/ServiceLocator.hpp"

namespace Aether {
    Swapchain::Swapchain(int width, int height) {
        queryInfo(width, height);

        vk::SwapchainCreateInfoKHR createInfo;
        createInfo.setClipped(true)
                  .setImageArrayLayers(1)
                  .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                  .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                  .setSurface(ServiceLocator::Get<VulkanContext>()->surface)
                  .setImageColorSpace(info.format.colorSpace)
                  .setImageFormat(info.format.format)
                  .setImageExtent(info.imageExtent)
                  .setMinImageCount(info.imageCount)
                  .setPresentMode(info.present);

        auto& queueIndices = ServiceLocator::Get<VulkanContext>()->queueFamilyIndices;
        if (queueIndices.graphics.value() == queueIndices.present.value()) {
            createInfo.setQueueFamilyIndices(queueIndices.graphics.value())
                      .setImageSharingMode(vk::SharingMode::eExclusive);
        } else {
            std::array indices = {queueIndices.graphics.value(), queueIndices.present.value()};
            createInfo.setQueueFamilyIndices(indices)
                      .setImageSharingMode(vk::SharingMode::eConcurrent);
        }

        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        auto& allocCallbacks = ServiceLocator::Get<VulkanContext>()->allocCallbacks;
        vk::Result result = device.createSwapchainKHR(&createInfo, allocCallbacks, &swapchain);
        if (result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create swapchain!");
        }

        getImages();
        createImageViews();
    }

    Swapchain::~Swapchain() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        for (const auto& framebuffer : framebuffers) {
            device.destroyFramebuffer(framebuffer);
        }

        for (const auto& imageView : imageViews) {
            device.destroyImageView(imageView);
        }

        device.destroySwapchainKHR(swapchain);
    }

    void Swapchain::queryInfo(int width, int height) {
        auto& phyDevice = ServiceLocator::Get<VulkanContext>()->phyDevice;
        auto& surface = ServiceLocator::Get<VulkanContext>()->surface;
        auto formats = phyDevice.getSurfaceFormatsKHR(surface);

        info.format = formats[0];
        for (const auto& format : formats) {
            if (format.format == vk::Format::eR8G8B8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                info.format = format;
                break;
            }
        }
        auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
        info.imageCount = std::clamp<uint32_t >(2, capabilities.minImageCount, capabilities.maxImageCount);

        info.imageExtent.width = std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        info.imageExtent.height = std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        info.transform = capabilities.currentTransform;

        auto presents = phyDevice.getSurfacePresentModesKHR(surface);
        info.present = vk::PresentModeKHR::eFifo;
        for (const auto& present : presents) {
            if (present == vk::PresentModeKHR::eMailbox) {
                info.present = present;
                break;
            }
        }
    }

    void Swapchain::getImages() {
        images = ServiceLocator::Get<VulkanContext>()->device.getSwapchainImagesKHR(swapchain);
    }

    void Swapchain::createImageViews() {
        imageViews.resize(images.size());

        for (size_t i = 0; i < images.size(); i++) {
            vk::ImageViewCreateInfo createInfo;
            vk::ComponentMapping mapping;
            vk::ImageSubresourceRange range;
            range.setBaseMipLevel(0)
                 .setLevelCount(1)
                 .setBaseArrayLayer(0)
                 .setLayerCount(1)
                 .setAspectMask(vk::ImageAspectFlagBits::eColor);
            createInfo.setImage(images[i])
                      .setComponents(mapping)
                      .setViewType(vk::ImageViewType::e2D)
                      .setFormat(info.format.format)
                      .setSubresourceRange(range);
            imageViews[i] = ServiceLocator::Get<VulkanContext>()->device.createImageView(createInfo);
        }
    }

    void Swapchain::createFramebuffers(int width, int height) {
        framebuffers.resize(images.size());
        for (int i = 0; i < framebuffers.size(); i++) {
            vk::FramebufferCreateInfo createInfo;
            createInfo.setAttachments(imageViews[i])
                      .setHeight(height)
                      .setWidth(width)
                      .setRenderPass(ServiceLocator::Get<VulkanContext>()->renderProcess->renderPass)
                      .setLayers(1);

            framebuffers[i] = ServiceLocator::Get<VulkanContext>()->device.createFramebuffer(createInfo);
        }
    }
}