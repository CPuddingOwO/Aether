#include <Aether/Core/Render/Shader.hpp>
#include <Aether/Core/Contexts/VulkanContext.hpp>
#include "Aether/Core/ServiceLocator/ServiceLocator.hpp"

namespace Aether {

    void Shader::initialize(const std::string& vertCode, const std::string& fragCode) {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = vertCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(vertCode.data());

        vertModule = device.createShaderModule(createInfo);

        createInfo.codeSize = fragCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(fragCode.data());
        fragModule = device.createShaderModule(createInfo);

        initStages();
    }

    void Shader::terminate() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        device.destroyShaderModule(vertModule);
        device.destroyShaderModule(fragModule);
    }

    std::vector<vk::PipelineShaderStageCreateInfo> Shader::GetStage() {
        return stages;
    }

    void Shader::initStages() {
        stages.resize(2);
        stages[0].setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertModule)
            .setPName("main");
        stages[1].setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragModule)
            .setPName("main");

    }

}