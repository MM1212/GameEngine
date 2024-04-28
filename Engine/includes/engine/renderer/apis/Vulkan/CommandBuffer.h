#pragma once

#include "defines.h"
#include "Device.h"
#include "Fence.h"

#include <glm/glm.hpp>
#include <vector>

namespace Engine::Renderers::Vulkan {
  enum class CommandBufferState {
    Ready,
    Recording,
    InRenderPass,
    RecordingEnded,
    Submitted,
    NotAllocated
  };
  struct CommandBufferSubmitInfo {
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_NONE;
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkSemaphore> signalSemaphores;
    bool resetFence = true;
    Fence* fence = nullptr;
  };
  class CommandBuffer {
  public:
    using State = CommandBufferState;
    CommandBuffer(Device& device, VkCommandPool pool, bool isPrimary = true);
    CommandBuffer(Device& device, VkCommandPool pool, VkCommandBuffer handle, VkCommandBufferLevel level);
    
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&& other) = default;
    CommandBuffer& operator=(CommandBuffer&& other) = default;

    ~CommandBuffer();

    operator VkCommandBuffer() const { return this->handle; }
    VkCommandBuffer getHandle() const { return this->handle; }
    VkCommandPool getPool() const { return this->pool; }
    State getState() const { return this->state; }

    CommandBuffer& beginRecording(
      bool oneTimeSubmit = false,
      bool renderPassInline = false,
      bool simultaneousUse = false
    );
    CommandBuffer& beginRecording(VkCommandBufferUsageFlags flags);
    virtual CommandBuffer& endRecording();
    void setAsSubmitted() { this->state = State::Submitted; }
    CommandBuffer& reset(bool releaseResources = false);
    void submit(VkQueue queue, CommandBufferSubmitInfo info = {});

    static std::vector<CommandBuffer> CreateMultiple(
      Device& device,
      VkCommandPool pool,
      uint32_t count,
      bool isPrimary = true
    );
    static void CreateMultiple(
      std::vector<CommandBuffer>& buffers,
      Device& device,
      VkCommandPool pool,
      uint32_t count,
      bool isPrimary = true
    );
  protected:
    void init();
    void free();
  protected:
    Device& device;
    VkCommandBuffer handle = VK_NULL_HANDLE;
    VkCommandPool pool;
    VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    State state = State::NotAllocated;
  };

  // Command buffer that starts right away in a recording state
  // when endRecording is called (or destructor), it will be submitted and discarded
  class DiscardableCommandBuffer : CommandBuffer {
  public:
    DiscardableCommandBuffer(
      Device& device,
      VkCommandPool pool,
      VkQueue queue,
      CommandBufferSubmitInfo info = {},
      bool isPrimary = true
    );
    ~DiscardableCommandBuffer();
    DiscardableCommandBuffer& endRecording() override;
  private:
    VkQueue submitQueue;
    CommandBufferSubmitInfo submitInfo;
  };
}