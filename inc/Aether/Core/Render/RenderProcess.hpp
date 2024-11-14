#pragma once

#include <vulkan/vulkan.hpp>

namespace Aether {
    class RenderProcess {
    public:
        RenderProcess() = default;
        ~RenderProcess();

        vk::Pipeline pipeline;
        vk::PipelineLayout layout;
        vk::RenderPass renderPass;

        void initializeLayout();
        void initializeRenderPass();
        void initializePipeline(int width, int height);

    };
}