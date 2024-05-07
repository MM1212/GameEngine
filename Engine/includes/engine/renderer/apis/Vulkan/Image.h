#pragma once

#include "defines.h"
#include "Device.h"
#include "CommandBuffer.h"

namespace Engine::Renderers::Vulkan {
  struct ImageViewCreateInfo {
    union {
      VkImageAspectFlags aspectMask;
      VkImageSubresourceRange subresourceRange = { 0, 0, 1, 0, 1 };
    };
    VkFormat format = VK_FORMAT_UNDEFINED;
  };
  struct ImageCreateInfo {
    VkImageType type;
    VkExtent3D extent = { 0, 0, 1 };
    uint32_t mipLevels = 1;
    uint32_t arrayLayers = 1;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags memoryProperties;
    bool createView = false;
    union {
      VkImageAspectFlags viewAspectFlags;
      ImageViewCreateInfo viewCreateInfo{};
    };
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  };
  class Image {
  public:
    Image(Device& device, const ImageCreateInfo& createInfo);
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&);
    Image& operator=(Image&&);
    ~Image();

    VkImage getHandle() const { return this->handle; }
    bool hasView() const { return this->view != VK_NULL_HANDLE; }
    VkImageView getView() const { return this->view; }
    const glm::uvec2& getSize() const { return this->size; }
    uint32_t getWidth() const { return this->size.x; }
    uint32_t getHeight() const { return this->size.y; }
    VkFormat getFormat() const { return this->format; }
    VkImageAspectFlags getAspectFlags() const { return this->viewAspectFlags; }
    VkImageType getType() const { return this->type; }
    VkImageTiling getTiling() const { return this->tiling; }
    VkImageUsageFlags getUsage() const { return this->usage; }
    VkMemoryPropertyFlags getMemoryProperties() const { return this->memoryProperties; }

    void createView(const ImageViewCreateInfo& createViewInfo);
    void transitionLayout(
      VkImageLayout oldLayout,
      VkImageLayout newLayout
    ) {
      auto cmdBuffer = this->device.createGraphicsSingleTimeCmds();
      auto graphicsQueueFamily = this->device.getQueueFamilies().graphicsFamily;
      return this->transitionLayout(cmdBuffer, graphicsQueueFamily, oldLayout, newLayout);
    }
    void transitionLayout(
      CommandBuffer& cmdBuffer,
      VkImageLayout oldLayout,
      VkImageLayout newLayout
    ) {
      auto graphicsQueueFamily = this->device.getQueueFamilies().graphicsFamily;
      return this->transitionLayout(cmdBuffer, graphicsQueueFamily, oldLayout, newLayout);
    }
    void transitionLayout(
      CommandBuffer& cmdBuffer,
      uint32_t queueFamilyIndex,
      VkImageLayout oldLayout,
      VkImageLayout newLayout
    );
    void copyFromBuffer(
      VkBuffer buffer,
      VkDeviceSize bufferOffset = 0
    ) {
      auto cmdBuffer = this->device.createGraphicsSingleTimeCmds();
      return this->copyFromBuffer(cmdBuffer, buffer, bufferOffset);
    }
    void copyFromBuffer(
      CommandBuffer& cmdBuffer,
      VkBuffer buffer,
      VkDeviceSize bufferOffset = 0
    );
  private:
    void init(const ImageCreateInfo& createInfo);
  private:
    Device& device;
    VkImageType type;
    union {
      glm::uvec2 size;
      struct {
        uint32_t width;
        uint32_t height;
      };
    };
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags memoryProperties;
    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkImageAspectFlags viewAspectFlags = VK_IMAGE_ASPECT_NONE;
  };
};