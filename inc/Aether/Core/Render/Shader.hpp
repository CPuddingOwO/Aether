#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace Aether {

    class Shader final {
    public:
        void initialize(const std::string& vertCode, const std::string& fragCode);
        void terminate();

        vk::ShaderModule vertModule;
        vk::ShaderModule fragModule;

        Shader() = default;
        ~Shader() = default;

        std::vector<vk::PipelineShaderStageCreateInfo> GetStage();

    private:

        std::vector<vk::PipelineShaderStageCreateInfo> stages;
        void initStages();
    };

}
