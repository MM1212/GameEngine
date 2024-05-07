#pragma once

#include "renderer/FrameInfo.h"
#include "utils.h"
#include "Device.h"
#include "Swapchain.h"
#include "RenderPass.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Fence.h"
#include "Semaphore.h"
#include "MemBuffer.h"

// #include "shaders/Object.h"
namespace Engine::Renderers::Vulkan::Shaders {
  class Object;
}

#include <core/Application.h>
#include <engine/renderer/RendererAPI.h>
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

    float getAspectRatio() const override {
      return this->swapchain->getAspectRatio();
    }
    glm::uvec2 getViewportSize() const override {
      return { this->swapchain->width(), this->swapchain->height() };
    }

    bool beginFrame(FrameInfo& frameInfo) override;
    bool endFrame(FrameInfo& frameInfo) override;
    void onResize(uint32_t width, uint32_t height) override;

    Ref<Engine::Texture2D> createTexture2D(const TextureSpecification& spec) override;
    Ref<Engine::Texture2D> createTexture2D(const std::string_view& path) override;

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

  public:
    static Renderer* Get() {
      ASSERT(Engine::Renderer::GetAPI() == Engine::Renderer::API::Vulkan, "Invalid renderer API");
      return reinterpret_cast<Renderer*>(Engine::Renderer::instance);
    }
  };
}