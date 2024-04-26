#pragma once

#include "defines.h"
#include "Device.h"
#include "CommandBuffer.h"

#include <glm/glm.hpp>

namespace Engine::Renderers::Vulkan {
  class Renderer;
  enum class RenderPassState {
    Ready,
    Recording,
    InRenderPass,
    RecordingEnded,
    Submitted,
    NotAllocated
  };
  struct RenderPassRenderArea {
    glm::ivec2 offset;
    union {
      glm::uvec2 size;
      struct {
        uint32_t width;
        uint32_t height;
      };
    };
  };
  struct RenderPassCreateInfo {
    RenderPassRenderArea renderArea;
    glm::vec4 clearColor;
    float depth;
    uint32_t stencil;
  };
  class Swapchain;
  class RenderPass {
  public:
    using State = RenderPassState;
    using RenderArea = RenderPassRenderArea;
    using CreateInfo = RenderPassCreateInfo;
    RenderPass(Device& device, Swapchain& swapchain, const RenderPassCreateInfo& createInfo);
    ~RenderPass();
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    void begin(CommandBuffer& cmdBuffer, VkFramebuffer frameBuffer, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
    void end(CommandBuffer& cmdBuffer);

    operator VkRenderPass() const { return this->handle; }
    VkRenderPass getHandle() const { return this->handle; }
    State getState() const { return this->state; }
    const RenderArea& getRenderArea() const { return this->renderArea; }
    const glm::vec4& getClearColor() const { return this->clearColor; }
    float getDepth() const { return this->depth; }
    uint32_t getStencil() const { return this->stencil; }
  private:
    void init();
  private:
    Device& device;
    Swapchain& swapchain;
    VkRenderPass handle;
    State state = State::NotAllocated;

    RenderArea renderArea;
    glm::vec4 clearColor;
    float depth;
    uint32_t stencil;
  };
}