#pragma once

#include "renderer/RendererAPI.h"
#include "Device.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Fence.h"
#include "Semaphore.h"

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
  private:
    VkExtent2D getWindowExtent() const {
      return { platform.window->getWidth(), platform.window->getHeight() };
    }
    void init();
    void recreateSwapchain();
    void createGraphicsCommandBuffers();
    void recreateOnResize();
    void createSyncObjects();

  private:
    Vulkan::Device device;

    Scope<Swapchain> swapchain = nullptr;

    std::vector<CommandBuffer> graphicsCommandBuffers;
    std::vector<Semaphore> imageAvailableSemaphores;
    std::vector<Semaphore> renderFinishedSemaphores;
    std::vector<Fence> inFlightFences;
    std::vector<Fence*> imagesInFlightFences;

    uint32_t currentImageIndex = 0;
    uint32_t currentFrameIndex = 0;
    bool isFrameStarted = false;
  };
}