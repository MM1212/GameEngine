#pragma once

#include "defines.h"
#include "Swapchain.h"
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
  class RenderPass {
  public:
    using State = RenderPassState;
    using RenderArea = RenderPassRenderArea;
    using CreateInfo = RenderPassCreateInfo;
    RenderPass(Renderer& renderer, const RenderPassCreateInfo& createInfo);
    ~RenderPass();
    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;

    void begin(CommandBuffer& cmdBuffer, VkFramebuffer frameBuffer, VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
    void end(CommandBuffer& cmdBuffer);
  private:
    Swapchain& swapchain() const;
    Device& device() const;
    void init();
  private:
    Renderer& renderer;
    VkRenderPass handle;
    State state = State::NotAllocated;

    RenderArea renderArea;
    glm::vec4 clearColor;
    float depth;
    uint32_t stencil;
  };
}