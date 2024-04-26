#include "renderer/apis/Vulkan/Framebuffer.h"

using namespace Engine::Renderers::Vulkan;

Framebuffer::Framebuffer(Device& device, CreateInfo createInfo)
  : device(device), renderPass(createInfo.renderPass), size(createInfo.size), attachments(createInfo.attachments) {
  this->init();
}

Framebuffer::Framebuffer(Device& device, VkRenderPass renderPass, glm::uvec2 size)
  : device(device), renderPass(renderPass), size(size) {
  this->init();
}

Framebuffer::Framebuffer(
  Device& device,
  VkRenderPass renderPass,
  uint32_t width,
  uint32_t height
)
  : device(device), renderPass(renderPass), size({ width, height }) {
  this->init();
}

Framebuffer::Framebuffer(
  Device& device,
  VkRenderPass renderPass,
  glm::uvec2 size,
  const std::vector<VkImageView>& attachments
)
  : device(device), renderPass(renderPass), size(size), attachments(attachments) {
  this->init();
}

Framebuffer::Framebuffer(
  Device& device,
  VkRenderPass renderPass,
  uint32_t width, uint32_t height,
  const std::vector<VkImageView>& attachments
) :
  device(device), renderPass(renderPass),
  size({ width, height }), attachments(attachments) {
  this->init();
}

Framebuffer::~Framebuffer() {
  vkDestroyFramebuffer(this->device, this->handle, this->device.getAllocator());
}

void Framebuffer::init() {
  VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
  createInfo.renderPass = this->renderPass;
  createInfo.attachmentCount = static_cast<uint32_t>(this->attachments.size());
  createInfo.pAttachments = this->attachments.data();
  createInfo.width = this->size.x;
  createInfo.height = this->size.y;
  createInfo.layers = 1;

  VK_CHECK(vkCreateFramebuffer(
    this->device,
    &createInfo,
    this->device.getAllocator(),
    &this->handle
  ));
}