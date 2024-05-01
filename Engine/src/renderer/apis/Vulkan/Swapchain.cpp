#include "renderer/apis/Vulkan/Swapchain.h"
#include "renderer/apis/Vulkan/RenderPass.h"
#include <renderer/logger.h>

using namespace Engine::Renderers::Vulkan;

Swapchain::Swapchain(
  Device& device,
  const SwapchainCreateInfo& createInfo
)
  : device(device),
  windowExtent(createInfo.windowExtent),
  vSync(createInfo.vSync),
  oldSwapchain(createInfo.oldSwapchain) {
  this->init(createInfo);
  this->oldSwapchain = nullptr;
}


Swapchain::~Swapchain() {
  this->device.waitIdle();
  for (auto imageView : this->imageViews)
    vkDestroyImageView(this->device, imageView, this->device.getAllocator());
  vkDestroySwapchainKHR(this->device, this->handle, this->device.getAllocator());
  LOG_RENDERER_INFO("Swapchain destroyed");

}

void Swapchain::init(const SwapchainCreateInfo& createInfo) {
  this->createSwapChain();
  this->createImageViews();
  this->createDepthResources();
  this->createMainRenderPass(createInfo.mainRenderPassCreateInfo);
  this->createFramebuffers();
  // this->createSyncObjects();
  LOG_RENDERER_INFO("Swapchain initialized");
}

void Swapchain::createSwapChain() {
  auto& support = this->device.getSwapChainSupport();
  auto surfaceFormat = this->chooseSurfaceFormat(support.formats);
  auto presentMode = this->choosePresentMode(support.presentModes);
  // might have changed
  this->device.querySwapChainSupport();
  auto extent = this->chooseExtent(support.capabilities);

  uint32_t imageCount = std::min(support.capabilities.minImageCount + 1, support.capabilities.maxImageCount);

  VkSwapchainCreateInfoKHR createInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
  createInfo.surface = this->device.getSurface();
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  auto& indices = this->device.getQueueFamilies();
  uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };
  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = support.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = this->oldSwapchain ? static_cast<VkSwapchainKHR>(*this->oldSwapchain) : VK_NULL_HANDLE;

  VK_CHECK(vkCreateSwapchainKHR(this->device, &createInfo, this->device.getAllocator(), &this->handle));
  this->imageFormat = surfaceFormat;
  this->swapChainExtent = extent;

  vkGetSwapchainImagesKHR(this->device, this->handle, &imageCount, nullptr);
  this->images.resize(imageCount);
  vkGetSwapchainImagesKHR(this->device, this->handle, &imageCount, this->images.data());
  this->maxFramesInFlight = std::max(1u, imageCount - 1);
}

void Swapchain::createImageViews() {
  this->imageViews.resize(this->images.size());
  for (size_t i = 0; i < this->images.size(); i++) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = this->images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = this->imageFormat.format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(this->device, &viewInfo, this->device.getAllocator(), &this->imageViews[i]));
  }
}

void Swapchain::createDepthResources() {
  this->depthFormat = this->findDepthFormat();
  this->depthImages.reserve(this->images.size());

  for (uint32_t i = 0; i < this->images.size(); i++) {
    ImageCreateInfo createInfo{};
    createInfo.type = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = this->swapChainExtent.width;
    createInfo.extent.height = this->swapChainExtent.height;
    createInfo.format = this->depthFormat;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    createInfo.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createInfo.createView = true;
    createInfo.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    createInfo.viewCreateInfo.subresourceRange.baseMipLevel = 0;
    createInfo.viewCreateInfo.subresourceRange.levelCount = 1;
    createInfo.viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.viewCreateInfo.subresourceRange.layerCount = 1;
    this->depthImages.emplace_back(this->device, createInfo);
  }
}

void Swapchain::createMainRenderPass(const RenderPassCreateInfo& baseCreateInfo) {
  RenderPassCreateInfo createInfo = baseCreateInfo;
  createInfo.renderArea.size = { this->swapChainExtent.width, this->swapChainExtent.height };

  this->mainRenderPass = MakeScope<RenderPass>(this->device, *this, createInfo);
}

void Swapchain::createFramebuffers() {
  this->framebuffers.reserve(this->getImageCount());
  for (uint32_t i = 0; i < this->getImageCount(); i++) {
    FramebufferCreateInfo createInfo;
    createInfo.renderPass = *this->mainRenderPass;
    createInfo.size = { this->swapChainExtent.width, this->swapChainExtent.height };
    createInfo.attachments = { this->getImageView(i), this->getDepthImageView(i) };
    this->framebuffers.emplace_back(this->device, createInfo);
  }
}

// we'll do sanity checks on the VkResult when we call this function
VkResult Swapchain::acquireNextImage(
  uint32_t* imageIndex,
  Semaphore& imageAvailableSemaphore,
  Fence& fence,
  uint64_t timeout
) {
  fence.wait(timeout);

  return vkAcquireNextImageKHR(
    this->device,
    this->handle, timeout,
    imageAvailableSemaphore, nullptr,
    imageIndex
  );
}

// we'll do sanity checks on the VkResult when we call this function
VkResult Swapchain::presentImage(uint32_t imageIndex, VkSemaphore renderFinishedSemaphore) {
  VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &this->handle;
  presentInfo.pImageIndices = &imageIndex;

  auto result = vkQueuePresentKHR(this->device.getPresentQueue(), &presentInfo);
  this->currentFrame = (this->currentFrame + 1) % this->maxFramesInFlight;
  return result;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(
  const std::vector<VkSurfaceFormatKHR>& availableFormats
) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
      availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR Swapchain::choosePresentMode(
  const std::vector<VkPresentModeKHR>& availablePresentModes
) {
  if (!this->vSync) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        LOG_RENDERER_INFO("Present mode: Mailbox");
        return availablePresentMode;
      }
    }
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        LOG_RENDERER_INFO("Present mode: Immediate");
        return availablePresentMode;
      }
    }
  }

  LOG_RENDERER_INFO("Present mode: V-Sync");
  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    return capabilities.currentExtent;

  VkExtent2D actualExtent = this->windowExtent;
  actualExtent.width = std::max(
    capabilities.minImageExtent.width,
    std::min(capabilities.maxImageExtent.width, actualExtent.width)
  );
  actualExtent.height = std::max(
    capabilities.minImageExtent.height,
    std::min(capabilities.maxImageExtent.height, actualExtent.height)
  );
  return actualExtent;
}

VkFormat Swapchain::findDepthFormat() const {
  return device.findSupportedFormat(
    { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );
}