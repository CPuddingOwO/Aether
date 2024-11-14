#pragma once

#include <vulkan/vulkan.hpp>

namespace Aether {

    class Buffer final {
    public:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        size_t size;

        Buffer(size_t size_, vk::BufferUsageFlagBits usage, vk::Flags<vk::MemoryPropertyFlagBits> property);
        ~Buffer();
    private:
        struct MemoryInfo final {
            size_t size;
            uint32_t index;

        };

        void createBuffer(vk::BufferUsageFlags usage);
        void allocateMemory(MemoryInfo info);
        void bindMemory2Buffer();
        MemoryInfo queryMemoryInfo(vk::Flags<vk::MemoryPropertyFlagBits> property);
    };

}