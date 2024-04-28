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

#include "shaders/Object.h"

#include <core/Application.h>
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine::Renderers::Vulkan {
  class Renderer : public RendererAPI {
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
    CommandBuffer& getCurrentGraphicsCommandBuffer() { return this->graphicsCommandBuffers[this->currentImageIndex]; }
  private:
    VkExtent2D getWindowExtent() const {
      return { platform.window->getWidth(), platform.window->getHeight() };
    }
    void init();
    bool recreateSwapchain();
    void createGraphicsCommandBuffers();
    void createSyncObjects();

  private:
    Vulkan::Device device;

    Scope<Swapchain> swapchain = nullptr;
    bool recreateSwapchainFlag = false;

    std::vector<CommandBuffer> graphicsCommandBuffers;
    std::vector<Semaphore> imageAvailableSemaphores;
    std::vector<Semaphore> renderFinishedSemaphores;
    std::vector<Fence> inFlightFences;
    std::vector<Fence*> imagesInFlightFences;

    Shaders::Object objectShader;

    uint32_t currentImageIndex = 0;
    uint32_t currentFrameIndex = 0;
    bool hasFrameStarted = false;
  };
}