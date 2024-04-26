#include "renderer/apis/Vulkan/VulkanRenderer.h"
#include <core/EngineInfo.h>
#include <renderer/logger.h>

using Engine::Renderers::Vulkan::Renderer;

Renderer::Renderer(ApplicationInfo& appInfo, Platform& platform)
  : RendererAPI(appInfo, platform, RendererAPI::API::Vulkan),
  device(appInfo, *platform.window) {
  this->init();
}

Renderer::~Renderer() {
  this->device.waitIdle();
  this->imageAvailableSemaphores.clear();
  this->renderFinishedSemaphores.clear();
  this->inFlightFences.clear();
  this->graphicsCommandBuffers.clear();
  this->swapchain.reset();
  vkDestroyCommandPool(this->device, this->device.getGraphicsCommandPool(), this->device.getAllocator());
}

void Renderer::init() {
  this->recreateSwapchain();
  this->createGraphicsCommandBuffers();
  this->createSyncObjects();
}

bool Renderer::beginFrame() {
  return true;
}

bool Renderer::endFrame() {
  return true;
}

void Renderer::onResize(uint32_t width, uint32_t height) {}

void Renderer::recreateOnResize() {
  this->device.waitIdle();
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

  auto& renderPassCreateInfo = createInfo.mainRenderPassCreateInfo;
  renderPassCreateInfo.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
  renderPassCreateInfo.depth = 1.0f;
  renderPassCreateInfo.stencil = 0;
 
  if (!this->swapchain)
    this->swapchain = MakeScope<Swapchain>(this->device, createInfo);
  else {
    createInfo.oldSwapchain = std::move(this->swapchain);
    this->swapchain = MakeScope<Swapchain>(this->device, createInfo);
    ASSERT(this->swapchain->compareFormats(*createInfo.oldSwapchain), "Swapchain image format has changed");
  }
}

void Renderer::createGraphicsCommandBuffers() {
  auto graphicsCommandPool = this->device.getGraphicsCommandPool();
  ASSERT(graphicsCommandPool != VK_NULL_HANDLE, "Invalid command pool");
  CommandBuffer::CreateMultiple(
    this->graphicsCommandBuffers,
    this->device,
    graphicsCommandPool,
    this->swapchain->getImageCount(),
    true
  );
  LOG_RENDERER_INFO("Created {} graphics command buffers", this->graphicsCommandBuffers.size());
}

void Renderer::createSyncObjects() {
  this->imageAvailableSemaphores.reserve(Swapchain::MAX_FRAMES_IN_FLIGHT);
  this->renderFinishedSemaphores.reserve(Swapchain::MAX_FRAMES_IN_FLIGHT);
  this->inFlightFences.reserve(Swapchain::MAX_FRAMES_IN_FLIGHT);
  this->imagesInFlightFences.resize(this->swapchain->getImageCount(), nullptr);

  for (size_t i = 0; i < Swapchain::MAX_FRAMES_IN_FLIGHT; i++) {
    this->imageAvailableSemaphores.emplace_back(this->device);
    this->renderFinishedSemaphores.emplace_back(this->device);
    this->inFlightFences.emplace_back(this->device, true);
  }
}