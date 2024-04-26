#pragma once

#include "defines.h"
#include "Device.h"

#include <glm/glm.hpp>

namespace Engine::Renderers::Vulkan {
  enum class CommandBufferState {
    Ready,
    Recording,
    InRenderPass,
    RecordingEnded,
    Submitted,
    NotAllocated
  };
  class CommandBuffer {
  public:
    using State = CommandBufferState;
    CommandBuffer(Device& device);
    ~CommandBuffer();
    VkCommandBuffer getHandle() const { return this->handle; }
  private:
    void init();
  private:
    Device& device;
    VkCommandBuffer handle;
    State state = State::NotAllocated;
  };
}