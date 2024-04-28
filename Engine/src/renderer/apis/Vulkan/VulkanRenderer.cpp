#include "renderer/apis/Vulkan/VulkanRenderer.h"
#include <core/EngineInfo.h>
#include <renderer/logger.h>

using Engine::Renderers::Vulkan::Renderer;

Renderer::Renderer(ApplicationInfo& appInfo, Platform& platform)
  : RendererAPI(appInfo, platform, RendererAPI::API::Vulkan),
  device(appInfo, *platform.window), objectShader(this->device) {
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
  ASSERT(!this->hasFrameStarted, "Renderer::beginFrame: Frame already started");
  if (this->recreateSwapchainFlag) {
    if (!this->recreateSwapchain()) {
      LOG_RENDERER_ERROR("Renderer::beginFrame: Failed to recreate swapchain");
    }
    this->recreateSwapchainFlag = false;
    return false;
  }

  auto result = this->swapchain->acquireNextImage(
    &this->currentImageIndex,
    this->imageAvailableSemaphores[this->currentFrameIndex],
    this->inFlightFences[this->currentFrameIndex]
  );
  ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR, "Failed to acquire next image");
  // for some reason, this might still happen, so force recreation again
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    this->recreateSwapchainFlag = true;
    return false;
  }
  this->hasFrameStarted = true;

  auto& cmdBuffer = this->getCurrentGraphicsCommandBuffer();
  cmdBuffer.reset().beginRecording();

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = static_cast<float>(this->swapchain->getExtent().height);
  viewport.width = static_cast<float>(this->swapchain->getExtent().width);
  viewport.height = -static_cast<float>(this->swapchain->getExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = { 0, 0 };
  scissor.extent = this->swapchain->getExtent();

  vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
  vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

  this->swapchain->getMainRenderPass().begin(
    cmdBuffer,
    this->swapchain->getFramebuffer(this->currentImageIndex)
  );

  return true;
}

bool Renderer::endFrame() {
  ASSERT(this->hasFrameStarted, "Renderer::endFrame: Frame not started");
  auto& cmdBuffer = this->getCurrentGraphicsCommandBuffer();

  this->swapchain->getMainRenderPass().end(cmdBuffer);
  cmdBuffer.endRecording();

  if (this->imagesInFlightFences[this->currentImageIndex])
    this->imagesInFlightFences[this->currentImageIndex]->wait();
  this->imagesInFlightFences[this->currentImageIndex] = &this->inFlightFences[this->currentFrameIndex];

  CommandBufferSubmitInfo submitInfo{};
  submitInfo.fence = &this->inFlightFences[this->currentFrameIndex];
  submitInfo.waitSemaphores = { this->imageAvailableSemaphores[this->currentFrameIndex] };
  submitInfo.signalSemaphores = { this->renderFinishedSemaphores[this->currentFrameIndex] };
  submitInfo.waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  submitInfo.resetFence = true;

  cmdBuffer.submit(this->device.getGraphicsQueue(), submitInfo);

  auto result = this->swapchain->presentImage(
    this->currentImageIndex,
    this->renderFinishedSemaphores[this->currentFrameIndex]
  );
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    this->recreateSwapchainFlag = true;
  }
  else if (!IsCallResultSuccess(result)) {
    LOG_RENDERER_ERROR("Renderer::endFrame: Failed to present image: {}", CallResultToString(result));
    return false;
  }

  this->currentFrameIndex = (this->currentFrameIndex + 1) % this->swapchain->getMaxFramesInFlight();
  this->hasFrameStarted = false;

  return true;
}

void Renderer::onResize(uint32_t width, uint32_t height) {
  this->recreateSwapchainFlag = true;
}

bool Renderer::recreateSwapchain() {
  VkExtent2D windowExtent = this->getWindowExtent();
  while (windowExtent.width == 0 || windowExtent.height == 0) {
    windowExtent = this->getWindowExtent();
    this->platform.window->waitEvents();
  }
  auto result = this->device.waitIdle();
  if (!IsCallResultSuccess(result)) {
    LOG_RENDERER_ERROR("Renderer::recreateSwapchain: Device::waitIdle(1) failed: {}", CallResultToString(result));
    return false;
  }
  this->device.querySwapChainSupport();

  SwapchainCreateInfo createInfo;
  createInfo.oldSwapchain = nullptr;
  createInfo.vSync = this->appInfo.windowInfo.vSync;
  createInfo.windowExtent = windowExtent;

  auto& renderPassCreateInfo = createInfo.mainRenderPassCreateInfo;
  renderPassCreateInfo.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
  renderPassCreateInfo.depth = 1.0f;
  renderPassCreateInfo.stencil = 0;

  try {
    if (!this->swapchain)
      this->swapchain = MakeScope<Swapchain>(this->device, createInfo);
    else {
      createInfo.oldSwapchain = std::move(this->swapchain);
      this->swapchain = MakeScope<Swapchain>(this->device, createInfo);
      ASSERT(this->swapchain->compareFormats(*createInfo.oldSwapchain), "Swapchain image format has changed");
    }
  }
  catch (const std::exception& e) {
    LOG_RENDERER_ERROR("Renderer::recreateSwapchain: {}", e.what());
    return false;
  }
  return true;
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
  auto maxFramesInFlight = this->swapchain->getMaxFramesInFlight();
  this->imageAvailableSemaphores.reserve(maxFramesInFlight);
  this->renderFinishedSemaphores.reserve(maxFramesInFlight);
  this->inFlightFences.reserve(maxFramesInFlight);
  this->imagesInFlightFences.resize(this->swapchain->getImageCount(), nullptr);

  for (size_t i = 0; i < maxFramesInFlight; i++) {
    this->imageAvailableSemaphores.emplace_back(this->device);
    this->renderFinishedSemaphores.emplace_back(this->device);
    this->inFlightFences.emplace_back(this->device, true);
  }
}