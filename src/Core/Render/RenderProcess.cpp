#include <Aether/Core/Render/RenderProcess.hpp>
#include "Aether/Core/Render/Shader.hpp"
#include "Aether/Core/ServiceLocator/ServiceLocator.hpp"
#include "Aether/Core/Contexts/VulkanContext.hpp"
#include <Aether/Core/Render/Vertext.hpp>

namespace Aether {

    void RenderProcess::initializePipeline(int width, int height) {
        vk::GraphicsPipelineCreateInfo createInfo;

        // 1. Vertex Input
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        auto attr = Vertex::GetAttribute();
        auto binding = Vertex::GetBinding();
        vertexInputInfo.setVertexAttributeDescriptions(attr)
                       .setVertexBindingDescriptions(binding);

        createInfo.setPVertexInputState(&vertexInputInfo);

        // 2. Input Assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.setPrimitiveRestartEnable(false)
                          .setTopology(vk::PrimitiveTopology::eTriangleList);
        createInfo.setPInputAssemblyState(&inputAssemblyInfo);

        // 3. Shader
        auto stages = ServiceLocator::Get<Shader>()->GetStage();
        createInfo.setStages(stages);

        // 4. Viewport
        vk::PipelineViewportStateCreateInfo viewportState;
        vk::Viewport viewport(0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);

        viewportState.setPViewports(&viewport);
        viewportState.setViewportCount(1);
        vk::Rect2D scissor({0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        viewportState.setScissors(scissor);
        createInfo.setPViewportState(&viewportState);

        // 5. Rasterization
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.setDepthClampEnable(false)
                         .setDepthBiasEnable(false)
                         .setRasterizerDiscardEnable(false)
                         .setPolygonMode(vk::PolygonMode::eFill)
                         .setFrontFace(vk::FrontFace::eCounterClockwise)
                         .setCullMode(vk::CullModeFlagBits::eBack)
                         .setLineWidth(1.0f);
        createInfo.setPRasterizationState(&rasterizationInfo);

        // 6. Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        multisampleInfo.setSampleShadingEnable(false)
                       .setRasterizationSamples(vk::SampleCountFlagBits::e1);
        createInfo.setPMultisampleState(&multisampleInfo);

        // 7. test - Depth & Stencil

        // 8. Color Blending
        vk::PipelineColorBlendAttachmentState attachs;
        attachs.setBlendEnable(false)
               .setColorWriteMask(vk::ColorComponentFlagBits::eA |
                                                vk::ColorComponentFlagBits::eB |
                                                vk::ColorComponentFlagBits::eG |
                                                vk::ColorComponentFlagBits::eR);

        vk::PipelineColorBlendStateCreateInfo blend;
        blend.setLogicOpEnable(false)
             .setAttachments(attachs);
        createInfo.setPColorBlendState(&blend);

        // 9. RenderPass & PipelineLayout
        createInfo.setLayout(layout)
                   .setRenderPass(renderPass);

        // Create Pipeline
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        auto& allocCallbacks = ServiceLocator::Get<VulkanContext>()->allocCallbacks;
        auto result = device.createGraphicsPipeline(nullptr, createInfo);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create graphics pipeline!");
        }else{
            pipeline = result.value;
        }
    }

    void RenderProcess::initializeLayout() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        auto& allocCallbacks = ServiceLocator::Get<VulkanContext>()->allocCallbacks;
        vk::PipelineLayoutCreateInfo layoutInfo;
        layout = device.createPipelineLayout(layoutInfo);
    }

    void RenderProcess::initializeRenderPass() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        vk::RenderPassCreateInfo createInfo;
        vk::AttachmentDescription attachDesc;
        attachDesc.setFormat(ServiceLocator::Get<VulkanContext>()->swapchain->info.format.format)
                  .setInitialLayout(vk::ImageLayout::eUndefined)
                  .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
                  .setLoadOp(vk::AttachmentLoadOp::eClear)
                  .setStoreOp(vk::AttachmentStoreOp::eStore)
                  .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                  .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                  .setSamples(vk::SampleCountFlagBits::e1);
        createInfo.setAttachments(attachDesc);

        vk::AttachmentReference reference;
        reference.setAttachment(0)
                .setLayout(vk::ImageLayout::eAttachmentOptimal);
        vk::SubpassDescription subpass;
        subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
               .setColorAttachments(reference);
        createInfo.setSubpasses(subpass);

        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
                  .setDstSubpass(0)
                  .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
                  .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
                  .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        createInfo.setDependencies(dependency);

        renderPass = device.createRenderPass(createInfo);
    }

    RenderProcess::~RenderProcess() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        device.destroyRenderPass(renderPass);
        device.destroyPipelineLayout(layout);
        device.destroyPipeline(pipeline);

    }
}