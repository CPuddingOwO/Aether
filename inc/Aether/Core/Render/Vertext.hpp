#pragma once

#include <vulkan/vulkan.hpp>

namespace Aether {

    struct Vertex final {
        float x, y;

        static vk::VertexInputAttributeDescription GetAttribute() {
            vk::VertexInputAttributeDescription attr;
            attr.setBinding(0)
                .setFormat(vk::Format::eR32G32Sfloat)
                .setLocation(0)
                .setOffset(0);
            return attr;
        }

        static vk::VertexInputBindingDescription GetBinding() {
            vk::VertexInputBindingDescription binding;
            binding.setBinding(0)
                   .setInputRate(vk::VertexInputRate::eInstance)
                    .setStride(sizeof(Vertex));
            return binding;
        }
    };

}
