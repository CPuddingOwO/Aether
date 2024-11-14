#include <Aether/Core/Render/Renderer.hpp>
#include <Aether/Core/Contexts/VulkanContext.hpp>
#include <Aether/Core/ServiceLocator/ServiceLocator.hpp>
#include <Aether/Core/Render/Vertext.hpp>
#include <memory>

namespace Aether {
    const std::array<Vertex, 3> vertices = {
            Vertex{0.5, 0.25},
            Vertex{0.25, 0.75},
            Vertex{0.75, 0.75},
    };


    void Renderer::initialize() {
        maxFlightCount = 2;
        curFrame = 0;
        createFence();
        createSemaphores();
        initializeCommandPool();
        allocateCommandBuffer();
        createVertexBuffer();
        bufferVertexData();
    }

    void Renderer::initializeCommandPool() {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        cmdPool = ServiceLocator::Get<VulkanContext>()->device.createCommandPool(createInfo);
    }

    void Renderer::terminate() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;

        device.freeCommandBuffers(cmdPool, cmdBuffers);
        device.destroyCommandPool(cmdPool);

        for (const auto& imageAvailableSemaphore : imageAvailableSemaphores)
            device.destroySemaphore(imageAvailableSemaphore);

        for (const auto& renderFinishedSemaphore : renderFinishedSemaphores)
            device.destroySemaphore(renderFinishedSemaphore);

        for (const auto& fence : cmdAvailableFences) device.destroyFence(fence);
    }

    void Renderer::allocateCommandBuffer() {
        cmdBuffers.resize(maxFlightCount, nullptr);
        for (auto& cmdBuffer : cmdBuffers) {
            vk::CommandBufferAllocateInfo allocInfo;
            allocInfo.setCommandPool(cmdPool)
                    .setCommandBufferCount(1)
                    .setLevel(vk::CommandBufferLevel::ePrimary);
            cmdBuffer = ServiceLocator::Get<VulkanContext>()->device.allocateCommandBuffers(allocInfo)[0];
        }
    }

    void Renderer::DrawTrangle() {

        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        auto& swapchain = ServiceLocator::Get<VulkanContext>()->swapchain;
        auto& renderProcess = ServiceLocator::Get<VulkanContext>()->renderProcess;

        if (device.waitForFences(cmdAvailableFences[curFrame], true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
            throw std::runtime_error("[Vulkan] Failed to wait for fence!");
        }
        device.resetFences(cmdAvailableFences[curFrame]);

        auto result = device.acquireNextImageKHR(swapchain->swapchain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphores[curFrame],
                                                 nullptr);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("[Vulkan] Failed to acquire next image!");
        }

        auto imageIndex = result.value;
        cmdBuffers[curFrame].reset();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuffers[curFrame].begin(beginInfo);
        {
            vk::RenderPassBeginInfo renderPassInfo;
            vk::Rect2D renderArea({0, 0}, swapchain->info.imageExtent);
            vk::ClearValue clearValue;
            clearValue.color = vk::ClearColorValue(0.1f, 0.1f, 0.1f, 1.0f);
            renderPassInfo.setRenderPass(renderProcess->renderPass)
                          .setRenderArea(renderArea)
                          .setFramebuffer(swapchain->framebuffers[imageIndex])
                          .setClearValues(clearValue);
            cmdBuffers[curFrame].beginRenderPass(renderPassInfo, {});
            cmdBuffers[curFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
            vk::DeviceSize offset = 0;
            cmdBuffers[curFrame].bindVertexBuffers(0, vertexBuffer->buffer, offset);
            cmdBuffers[curFrame].draw(3, 1, 0, 0);

            cmdBuffers[curFrame].endRenderPass();

        } cmdBuffers[curFrame].end();

        std::array<vk::PipelineStageFlags, 1> waitStageMask = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuffers[curFrame])
                .setWaitDstStageMask(waitStageMask)
                .setWaitSemaphores(imageAvailableSemaphores[curFrame])
                .setSignalSemaphores(renderFinishedSemaphores[curFrame]);
        ServiceLocator::Get<VulkanContext>()->graphicsQueue.submit(submitInfo, cmdAvailableFences[curFrame]);

        vk::PresentInfoKHR presentInfo;
        presentInfo.setSwapchains(swapchain->swapchain)
                   .setWaitSemaphores(renderFinishedSemaphores[curFrame])
                   .setImageIndices(imageIndex);
        if(ServiceLocator::Get<VulkanContext>()->presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("[Vulkan ]Failed to present image!");
        }

        curFrame = (curFrame + 1) % maxFlightCount;
    }

    void Renderer::createSemaphores() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        vk::SemaphoreCreateInfo createInfo;

        imageAvailableSemaphores.resize(maxFlightCount, nullptr);
        renderFinishedSemaphores.resize(maxFlightCount, nullptr);

        for (auto& semaphore : imageAvailableSemaphores) {
            semaphore = device.createSemaphore(createInfo);
        }

        for (auto& semaphore : renderFinishedSemaphores) {
            semaphore = device.createSemaphore(createInfo);
        }

    }

    void Renderer::createFence() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        cmdAvailableFences.resize(maxFlightCount, nullptr);
        for (auto& fence : cmdAvailableFences) {
            vk::FenceCreateInfo createInfo;
            createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            fence = device.createFence(createInfo);
        }
    }

    void Renderer::createVertexBuffer() {
        vertexBuffer = std::make_unique<Buffer>(
                sizeof (vertices),
                vk::BufferUsageFlagBits::eVertexBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                );
    }

    void Renderer::bufferVertexData() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        void* data = device.mapMemory(vertexBuffer->memory, 0, sizeof(vertexBuffer));
        memcpy(data, vertices.data(), sizeof(vertices));
        device.unmapMemory(vertexBuffer->memory);
    }
}

