#include "renderer/apis/Vulkan/VulkanRenderer.h"
#include "renderer/apis/Vulkan/RenderPass.h"
#include <renderer/logger.h>
#include <utils/asserts.h>

using namespace Engine::Renderers::Vulkan;

RenderPass::RenderPass(
  Device& device,
  Swapchain& swapchain,
  const RenderPassCreateInfo& createInfo
)
  : device(device), swapchain(swapchain),
  renderArea(createInfo.renderArea),
  clearColor(createInfo.clearColor),
  depth(createInfo.depth), stencil(createInfo.stencil) {
  this->init();
}

RenderPass::~RenderPass() {
  if (this->handle)
    vkDestroyRenderPass(this->device, this->handle, this->device.getAllocator());
}

void RenderPass::init() {
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = this->swapchain.getImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = this->swapchain.getDepthFormat();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

  VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
  createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  createInfo.pAttachments = attachments.data();
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpass;
  createInfo.dependencyCount = 1;
  createInfo.pDependencies = &dependency;

  VK_CHECK(vkCreateRenderPass(
    this->device,
    &createInfo,
    this->device.getAllocator(),
    &this->handle
  ));
}

void RenderPass::begin(CommandBuffer& cmdBuffer, VkFramebuffer frameBuffer, VkSubpassContents contents) {
  ASSERT(this->state == State::Ready, "RenderPass is not ready to begin recording");

  VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
  beginInfo.renderPass = this->handle;
  beginInfo.framebuffer = frameBuffer;
  beginInfo.renderArea.offset = { this->renderArea.offset.x, this->renderArea.offset.y };
  beginInfo.renderArea.extent = { this->renderArea.width, this->renderArea.height };

  std::array<VkClearValue, 2> clearValues = {};
  clearValues[0].color = { this->clearColor.r, this->clearColor.g, this->clearColor.b, this->clearColor.a };
  clearValues[1].depthStencil = { this->depth, this->stencil };

  beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  beginInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(cmdBuffer, &beginInfo, contents);
  this->state = State::InRenderPass;
}

void RenderPass::end(CommandBuffer& cmdBuffer) {
  ASSERT(this->state == State::InRenderPass, "RenderPass is not in render pass state");

  vkCmdEndRenderPass(cmdBuffer);
  this->state = State::RecordingEnded;
}