#include "renderer/apis/Vulkan/VulkanRenderer.h"
#include <core/EngineInfo.h>

using Engine::Renderers::Vulkan::Renderer;

Renderer::Renderer(ApplicationInfo& appInfo, Platform& platform)
  : RendererAPI(appInfo, platform, RendererAPI::API::Vulkan), device(appInfo, *platform.window) {
  this->init();
}

Renderer::~Renderer() {}

void Renderer::init() {
  this->recreateSwapchain();
}

void Renderer::recreateSwapchain() {
  VkExtent2D windowExtent = this->getWindowExtent();
  while (windowExtent.width == 0 || windowExtent.height == 0) {
    windowExtent = this->getWindowExtent();
    this->platform.window->waitEvents();
  }
  this->device.waitIdle();
  SwapchainCreateInfo createInfo;
  createInfo.oldSwapchain = nullptr;
  createInfo.vSync = this->appInfo.windowInfo.vSync;
  createInfo.windowExtent = windowExtent;
  if (!this->swapchain)
    this->swapchain = std::make_unique<Swapchain>(this->device, createInfo);
  else {
    createInfo.oldSwapchain = std::move(this->swapchain);
    this->swapchain = std::make_unique<Swapchain>(this->device, createInfo);
    ASSERT(this->swapchain->compareFormats(*createInfo.oldSwapchain), "Swapchain image format has changed");
  }
}

bool Renderer::beginFrame() {
  return true;
}

bool Renderer::endFrame() {
  return true;
}

void Renderer::onResize(uint32_t width, uint32_t height) {}

