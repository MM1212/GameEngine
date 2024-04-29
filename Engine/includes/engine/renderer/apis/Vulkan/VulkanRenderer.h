#pragma once

#include "renderer/RendererAPI.h"
#include "utils.h"
#include "Device.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Fence.h"
#include "Semaphore.h"
#include "MemBuffer.h"

#include "shaders/Object.h"

#include <core/Application.h>
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine::Renderers::Vulkan {
  class Renderer : public Engine::Renderer {
  public:
    Renderer() = delete;
    Renderer(ApplicationInfo& appInfo, Platform& platform);
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    ~Renderer() override;
    bool beginFrame() override;
    bool endFrame() override;
    void onResize(uint32_t width, uint32_t height) override;

    Device& getDevice() { return this->device; }
    Swapchain& getSwapchain() const { return *this->swapchain; }
    RenderPass& getMainRenderPass() const { return this->swapchain->getMainRenderPass(); }
    CommandBuffer& getCurrentGraphicsCommandBuffer() { return this->graphicsCommandBuffers[this->currentImageIndex]; }
  private:
    VkExtent2D getWindowExtent() const {
      return { platform.window->getWidth(), platform.window->getHeight() };
    }
    void init();
    bool recreateSwapchain();
    void createGraphicsCommandBuffers();
    void createSyncObjects();
    void createObjectBuffers();
    void uploadTestObjectData();

    // temp
    void uploadDataToBuffer(
      MemBuffer& buffer,
      VkQueue queue, VkCommandPool cmdPool, Fence* fence,
      const void* data, size_t size, uint64_t offset = 0
    );

  private:
    Vulkan::Device device;

    Scope<Swapchain> swapchain = nullptr;
    bool recreateSwapchainFlag = false;

    std::vector<CommandBuffer> graphicsCommandBuffers;
    std::vector<Semaphore> imageAvailableSemaphores;
    std::vector<Semaphore> renderFinishedSemaphores;
    std::vector<Fence> inFlightFences;
    std::vector<Fence*> imagesInFlightFences;

    Scope<Shaders::Object> objectShader = nullptr;
    Scope<MemBuffer> objectVertexBuffer = nullptr;
    Scope<MemBuffer> objectIndexBuffer = nullptr;
    uint64_t objectVertexOffset = 0;
    uint64_t objectIndexOffset = 0;


    uint32_t currentImageIndex = 0;
    uint32_t currentFrameIndex = 0;
    bool hasFrameStarted = false;
  };
}