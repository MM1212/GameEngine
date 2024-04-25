#include "renderer/apis/Vulkan/Swapchain.h"
#include <renderer/logger.h>

using namespace Engine::Renderers::Vulkan;

Swapchain::Swapchain(Device& device, SwapchainCreateInfo createInfo)
  : device(device), windowExtent(createInfo.windowExtent), vSync(createInfo.vSync), oldSwapchain(createInfo.oldSwapchain) {
  this->init();
}


Swapchain::~Swapchain() {
  this->device.waitIdle();
  for (auto imageView : this->imageViews)
    vkDestroyImageView(this->device.getHandle(), imageView, this->device.getAllocator());
  vkDestroySwapchainKHR(this->device.getHandle(), this->handle, this->device.getAllocator());
  LOG_RENDERER_INFO("Swapchain destroyed");
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(this->device.getHandle(), this->imageAvailableSemaphores[i], this->device.getAllocator());
    vkDestroySemaphore(this->device.getHandle(), this->renderFinishedSemaphores[i], this->device.getAllocator());
    vkDestroyFence(this->device.getHandle(), this->inFlightFences[i], this->device.getAllocator());
  }
}

void Swapchain::init() {
  this->createSwapChain();
  this->createImageViews();
  this->createDepthResources();
  this->createSyncObjects();
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
  createInfo.oldSwapchain = this->oldSwapchain ? this->oldSwapchain->getHandle() : VK_NULL_HANDLE;

  VK_CHECK(vkCreateSwapchainKHR(this->device.getHandle(), &createInfo, this->device.getAllocator(), &this->handle));
  this->imageFormat = surfaceFormat;
  this->swapChainExtent = extent;

  vkGetSwapchainImagesKHR(this->device.getHandle(), this->handle, &imageCount, nullptr);
  this->images.resize(imageCount);
  vkGetSwapchainImagesKHR(this->device.getHandle(), this->handle, &imageCount, this->images.data());
}

// we'll do sanity checks on the VkResult when we call this function
VkResult Swapchain::acquireNextImage(uint32_t* imageIndex, uint64_t timeout) {
  return vkAcquireNextImageKHR(
    this->device.getHandle(),
    this->handle, timeout,
    this->imageAvailableSemaphores[this->currentFrame],
    this->inFlightFences[this->currentFrame], imageIndex
  );
}

// we'll do sanity checks on the VkResult when we call this function
VkResult Swapchain::presentImage(uint32_t imageIndex) {
  VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &this->renderFinishedSemaphores[this->currentFrame];
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &this->handle;
  presentInfo.pImageIndices = &imageIndex;
  auto result = vkQueuePresentKHR(this->device.getQueues().present, &presentInfo);
  this->currentFrame = (this->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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

    VK_CHECK(vkCreateImageView(this->device.getHandle(), &viewInfo, this->device.getAllocator(), &this->imageViews[i]));
  }
}
VkFormat Swapchain::findDepthFormat() const {
  return device.findSupportedFormat(
    { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
    VK_IMAGE_TILING_OPTIMAL,
    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
  );
}

void Swapchain::createDepthResources() {
  this->depthFormat = this->findDepthFormat();
  this->depthImages.reserve(this->images.size());

  for (uint32_t i = 0; i < this->depthImages.size(); i++) {
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
    this->depthImages.emplace_back(this->device, createInfo);
  }
}

void Swapchain::createSyncObjects() {
  this->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  this->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  this->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  this->imagesInFlight.resize(this->images.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    VK_CHECK(vkCreateSemaphore(
      device.getHandle(), 
      &semaphoreInfo, 
      this->device.getAllocator(), 
      &this->imageAvailableSemaphores[i]
    ));
    VK_CHECK(vkCreateSemaphore(
      device.getHandle(), 
      &semaphoreInfo, 
      this->device.getAllocator(), 
      &this->renderFinishedSemaphores[i]
    ));
    VK_CHECK(vkCreateFence(
      device.getHandle(), 
      &fenceInfo, 
      this->device.getAllocator(), 
      &this->inFlightFences[i]
    ));
  }
}