#pragma once

#include "renderer/RendererAPI.h"
#include <vulkan/vulkan.h>
#include <core/Application.h>

namespace Engine {
  class VulkanRenderer : public Renderer {
  public:
    VulkanRenderer() = delete;
    VulkanRenderer(ApplicationInfo& appInfo, Platform& platform);
    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer(VulkanRenderer&&) = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;

    ~VulkanRenderer() override;
    bool beginFrame() override;
    bool endFrame() override;
    void onResize(uint32_t width, uint32_t height) override;
  private:
    void init();
  private:
    VkInstance instance = VK_NULL_HANDLE;
    VkAllocationCallbacks* allocator = nullptr;

  };
}