#pragma once

#include "defines.h"
#include "Device.h"

namespace Engine::Renderers::Vulkan {
  struct FramebufferCreateInfo {
    VkRenderPass renderPass = VK_NULL_HANDLE;
    glm::uvec2 size{};
    std::vector<VkImageView> attachments{};
  };
  class Framebuffer {
  public:
    using CreateInfo = FramebufferCreateInfo;
    Framebuffer(Device& device, CreateInfo createInfo);
    Framebuffer(Device& device, VkRenderPass renderPass, glm::uvec2 size);
    Framebuffer(Device& device, VkRenderPass renderPass, uint32_t width, uint32_t height);
    Framebuffer(Device& device, VkRenderPass renderPass, glm::uvec2 size, const std::vector<VkImageView>& attachments);
    Framebuffer(Device& device, VkRenderPass renderPass, uint32_t width, uint32_t height, const std::vector<VkImageView>& attachments);
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    Framebuffer(Framebuffer&& other) = default;
    Framebuffer& operator=(Framebuffer&& other) = default;

    operator VkFramebuffer() const { return this->handle; }
    VkFramebuffer getHandle() const { return this->handle; }
    VkRenderPass getRenderPass() const { return this->renderPass; }
    uint32_t width() const { return this->size.x; }
    uint32_t height() const { return this->size.y; }
    const std::vector<VkImageView>& getAttachments() const { return this->attachments; }
  private:
    void init();
  private:
    Device& device;
    VkFramebuffer handle = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    glm::uvec2 size;
    std::vector<VkImageView> attachments;
  };
}