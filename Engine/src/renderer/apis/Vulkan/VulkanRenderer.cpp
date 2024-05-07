#include "renderer/apis/Vulkan/VulkanRenderer.h"
#include "renderer/apis/Vulkan/shaders/Object.h"
#include "renderer/apis/Vulkan/Texture2D.h"

#include <core/EngineInfo.h>
#include <core/Coordinates.h>
#include <core/PoolManager.h>
#include <renderer/logger.h>

using Engine::Renderers::Vulkan::Renderer;

Renderer::Renderer(ApplicationInfo& appInfo, Platform& platform)
  : Engine::Renderer(appInfo, platform, Renderer::API::Vulkan),
  device(appInfo, *platform.window) {
  this->init();
  instance = this;
}

Renderer::~Renderer() {
  this->device.waitIdle();
  this->imageAvailableSemaphores.clear();
  this->renderFinishedSemaphores.clear();
  this->inFlightFences.clear();
  this->graphicsCommandBuffers.clear();
  this->swapchain.reset();
}

void Renderer::init() {
  this->recreateSwapchain();
  this->createGraphicsCommandBuffers();
  this->createSyncObjects();
  this->objectShader = MakeScope<Shaders::Object>(*this, this->getMainRenderPass());
  this->createObjectBuffers();
  this->uploadTestObjectData();
}

bool Renderer::beginFrame(FrameInfo& frameInfo) {
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
  VkFrameInfo vkFrameInfo{
    frameInfo,
    this->currentFrameIndex,
    this->getCurrentGraphicsCommandBuffer(),
    this->objectShader->getGlobalDescriptorSet(this->currentFrameIndex)
  };

  auto& cmdBuffer = vkFrameInfo.cmdBuffer;
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

  this->getMainRenderPass().begin(
    cmdBuffer,
    this->swapchain->getFramebuffer(this->currentImageIndex)
  );

  // TODO: tmp code
  this->objectShader->updateGlobalUniforms(vkFrameInfo);

  this->objectShader->use(vkFrameInfo);
  VkBuffer vertexBuffers[] = { this->objectVertexBuffer->getHandle() };
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
  vkCmdBindIndexBuffer(cmdBuffer, this->objectIndexBuffer->getHandle(), 0, VK_INDEX_TYPE_UINT32);

  // plane
  glm::vec3 offset{ 0.f, -5.f, 0.f };
  glm::vec3 scale{ 20.f, 1.f, 20.f };
  glm::mat4 planeModel = glm::translate(glm::mat4(1.f), offset) * glm::scale(glm::mat4(1.f), scale);
  vkCmdPushConstants(
    cmdBuffer,
    this->objectShader->getPipelineLayout(),
    VK_SHADER_STAGE_VERTEX_BIT,
    0, sizeof(glm::mat4), &planeModel
  );
  vkCmdDrawIndexed(cmdBuffer, 6, 1, 36, 0, 0);

  // static float angle = 0.f;
  // angle += 2.f * frameInfo.deltaTime;
  // glm::mat4 cubeModel = glm::rotate(glm::mat4(1.f), angle, Coordinates::Up<glm::vec3>);
  glm::mat4 cubeModel = glm::mat4{ 1.f };
  vkCmdPushConstants(
    cmdBuffer,
    this->objectShader->getPipelineLayout(),
    VK_SHADER_STAGE_VERTEX_BIT,
    0, sizeof(glm::mat4), &cubeModel
  );
  // cube
  vkCmdDrawIndexed(cmdBuffer, 36, 1, 0, 0, 0);

  return true;
}

bool Renderer::endFrame(FrameInfo& frameInfo) {
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

static Engine::PoolManager::Pool* vertexPool = nullptr;
static Engine::PoolManager::Pool* indexPool = nullptr;

void Renderer::createObjectBuffers() {
  vertexPool = Engine::PoolManager::Get<Engine::Pools::RendererVertices>();
  this->objectVertexBuffer = MakeScope<MemBuffer>(
    this->device,
    sizeof(Shaders::Object::Vertex),
    static_cast<uint32_t>(vertexPool->getMaxSize()),
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
  indexPool = Engine::PoolManager::Get<Engine::Pools::RendererIndices>();
  this->objectIndexBuffer = MakeScope<MemBuffer>(
    this->device,
    sizeof(uint32_t),
    static_cast<uint32_t>(indexPool->getMaxSize()),
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
  );
}

void Renderer::uploadDataToBuffer(
  MemBuffer& buffer,
  VkQueue queue, VkCommandPool cmdPool, Fence* fence,
  const void* data, size_t size, uint64_t offset
) {
  MemBuffer stagingBuffer(
    this->device,
    size,
    1,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );
  stagingBuffer.map();
  stagingBuffer.writeTo(data, size);
  stagingBuffer.unmap();
  this->device.copyBuffer(
    stagingBuffer, buffer,
    size,
    queue, cmdPool, fence,
    0, offset
  );
}

void Renderer::uploadTestObjectData() {
  constexpr float mult = 10.f;
  std::vector<Shaders::Object::Vertex> vertices = {
    // right face (white)
    {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
    {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
    {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

    // left face (yellow)
    {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
    {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
    {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

    // bottom face (orange)
    {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
    {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
    {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

    // top face (red)
    {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
    {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
    {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

    // tail face (blue)
    {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
    {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
    {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

    // nose face (green)
    {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
  };
  std::vector<uint32_t> indices = {
    2,  1,  0,  1,  3,  0, // right face
    4,  5,  6,  4,  7,  5, // left face
    8,  9,  10, 8,  11, 9, // bottom face
    14, 13, 12, 13, 15, 12, // top face
    16, 17, 18, 16, 19, 17, // tail face
    22, 21, 20, 21, 23, 20 // nose face
  };
  this->uploadDataToBuffer(
    *this->objectVertexBuffer,
    this->device.getGraphicsQueue(),
    this->device.getGraphicsCommandPool(),
    nullptr,
    vertices.data(),
    vertices.size() * sizeof(Shaders::Object::Vertex),
    this->objectVertexOffset
  );
  vertexPool += vertices.size();
  this->objectVertexOffset += vertices.size() * sizeof(Shaders::Object::Vertex);
  this->uploadDataToBuffer(
    *this->objectIndexBuffer,
    this->device.getGraphicsQueue(),
    this->device.getGraphicsCommandPool(),
    nullptr,
    indices.data(),
    indices.size() * sizeof(uint32_t),
    this->objectIndexOffset
  );
  indexPool += indices.size();
  this->objectIndexOffset += indices.size() * sizeof(uint32_t);

  std::vector<Shaders::Object::Vertex> planeVertices = {
    {{-.5f, 0.f, -.5f}, {.9f, .9f, .9f}},
    {{-.5f, 0.f, .5f}, {.9f, .9f, .9f}},
    {{.5f, 0.f, -.5f}, {.9f, .9f, .9f}},
    {{.5f, 0.f, .5f}, {.9f, .9f, .9f}},
  };
  std::vector<uint32_t> planeIndices = { 0, 1, 2, 2, 1, 3 };
  for (auto& index : planeIndices) {
    index += 24;
  }
  this->uploadDataToBuffer(
    *this->objectVertexBuffer,
    this->device.getGraphicsQueue(),
    this->device.getGraphicsCommandPool(),
    nullptr,
    planeVertices.data(),
    planeVertices.size() * sizeof(Shaders::Object::Vertex),
    this->objectVertexOffset
  );
  vertexPool += planeVertices.size();
  this->objectVertexOffset += planeVertices.size() * sizeof(Shaders::Object::Vertex);
  this->uploadDataToBuffer(
    *this->objectIndexBuffer,
    this->device.getGraphicsQueue(),
    this->device.getGraphicsCommandPool(),
    nullptr,
    planeIndices.data(),
    planeIndices.size() * sizeof(uint32_t),
    this->objectIndexOffset
  );
  indexPool += planeIndices.size();
  this->objectIndexOffset += planeIndices.size() * sizeof(uint32_t);
}

Engine::Ref<Engine::Texture2D> Renderer::createTexture2D(const TextureSpecification& spec) {
  return MakeRef<Texture2D>(this->device, spec);
}
Engine::Ref<Engine::Texture2D> Renderer::createTexture2D(const std::string_view& path) {
  ASSERT(false, "Not implemented");
  return nullptr;
}