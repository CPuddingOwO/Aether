#pragma once

#include <vulkan/vulkan.hpp>
#include <Aether/Core/Render/Buffer.hpp>

namespace Aether {

    class Renderer final {

    public:
        ~Renderer() { terminate(); };

        void initialize();
        void terminate();

        void DrawTrangle();

        void initializeCommandPool();
        void allocateCommandBuffer();

        void createSemaphores();
        void createFence();
    private:
        int maxFlightCount;
        int curFrame;

        vk::CommandPool cmdPool;
        std::vector<vk::CommandBuffer> cmdBuffers;
        std::vector<vk::Semaphore> imageAvailableSemaphores;
        std::vector<vk::Semaphore> renderFinishedSemaphores;
        std::vector<vk::Fence> cmdAvailableFences;

        std::unique_ptr<Buffer> vertexBuffer;

        void createVertexBuffer();
        void bufferVertexData();
    };

}