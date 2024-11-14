#include <Aether/Core/Render/Buffer.hpp>
#include <Aether/Core/ServiceLocator/ServiceLocator.hpp>
#include <Aether/Core/Contexts/VulkanContext.hpp>

namespace Aether {

    Buffer::Buffer(size_t size_, vk::BufferUsageFlagBits usage, vk::Flags<vk::MemoryPropertyFlagBits> property) {
        size = size_;
        createBuffer(usage);
        auto info = queryMemoryInfo(property);
        allocateMemory(info);
        bindMemory2Buffer();
    }

    Buffer::~Buffer() {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        device.freeMemory(memory);
        device.destroyBuffer(buffer);

    }

    void Buffer::createBuffer(vk::BufferUsageFlags usage) {
        vk::BufferCreateInfo createInfo;
        createInfo.setSize(size)
                  .setSharingMode(vk::SharingMode::eExclusive)
                  .setUsage(usage);

        buffer = ServiceLocator::Get<VulkanContext>()->device.createBuffer(createInfo);
    }

    void Buffer::allocateMemory(MemoryInfo info) {
        vk::MemoryAllocateInfo createInfo;
        createInfo.setAllocationSize(info.size)
                  .setMemoryTypeIndex(info.index);

        memory = ServiceLocator::Get<VulkanContext>()->device.allocateMemory(createInfo);
    }

    void Buffer::bindMemory2Buffer() {
        ServiceLocator::Get<VulkanContext>()->device.bindBufferMemory(buffer, memory, 0);
    }

    Buffer::MemoryInfo Buffer::queryMemoryInfo(vk::Flags<vk::MemoryPropertyFlagBits> property) {
        auto& device = ServiceLocator::Get<VulkanContext>()->device;
        auto& phyDevice = ServiceLocator::Get<VulkanContext>()->phyDevice;

        MemoryInfo memInfo{};
        auto requirements =  device.getBufferMemoryRequirements(buffer);
        memInfo.size = requirements.size;

        auto properties = phyDevice.getMemoryProperties();

        for (int i = 0; i < properties.memoryTypeCount; i++) {
            if ((1 << i) & requirements.memoryTypeBits &&
            properties.memoryTypes[i].propertyFlags & property) {
                memInfo.index = i;
                break;
            }
        }

        return memInfo;
    }

}