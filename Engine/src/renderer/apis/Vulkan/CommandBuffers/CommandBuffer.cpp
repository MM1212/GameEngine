#include "renderer/apis/Vulkan/CommandBuffer.h"
#include <renderer/apis/Vulkan/Device.h>

using namespace Engine::Renderers::Vulkan;

CommandBuffer::CommandBuffer(
  Device& device,
  VkCommandPool pool,
  bool isPrimary
)
  : device(device), pool(pool),
  level(isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY) {
  this->init();
}

CommandBuffer::CommandBuffer(
  Device& device,
  VkCommandPool pool,
  VkCommandBuffer handle,
  VkCommandBufferLevel level
) : device(device), pool(pool), handle(handle), level(level) {
  this->state = State::Ready;
}

CommandBuffer::~CommandBuffer() {
  this->free();
}

void CommandBuffer::free() {
  switch (this->state) {
    case State::NotAllocated:
      return;
    case State::Recording:
    case State::InRenderPass:
      this->endRecording();
      break;
    case State::Submitted:
      this->device.waitIdle();
      break;
    default:
      break;
  }
  vkFreeCommandBuffers(this->device, this->pool, 1, &this->handle);
  this->state = State::NotAllocated;
  this->handle = VK_NULL_HANDLE;
}

void CommandBuffer::init() {
  VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocInfo.commandPool = this->pool;
  allocInfo.level = this->level;
  allocInfo.commandBufferCount = 1;

  VK_CHECK(vkAllocateCommandBuffers(this->device, &allocInfo, &this->handle));
  this->state = State::Ready;
}

CommandBuffer& CommandBuffer::beginRecording(VkCommandBufferUsageFlags flags) {
  ASSERT(this->state == State::Ready, "Invalid command buffer state");
  VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  beginInfo.flags = flags;

  VK_CHECK(vkBeginCommandBuffer(this->handle, &beginInfo));
  this->state = State::Recording;
  return *this;
}

CommandBuffer& CommandBuffer::beginRecording(
  bool oneTimeSubmit,
  bool renderPassInline,
  bool simultaneousUse
) {
  VkCommandBufferUsageFlags flags = 0;
  if (oneTimeSubmit)
    flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  if (renderPassInline)
    flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  if (simultaneousUse)
    flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  return this->beginRecording(flags);
}

CommandBuffer& CommandBuffer::endRecording() {
  ASSERT(this->state == State::Recording || this->state == State::InRenderPass, "Invalid command buffer state");
  VK_CHECK(vkEndCommandBuffer(this->handle));
  this->state = State::RecordingEnded;
  return *this;
}

void CommandBuffer::submit(VkQueue queue, CommandBufferSubmitInfo info) {
  ASSERT(this->state == State::RecordingEnded, "Invalid command buffer state");
  VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &this->handle;
  submitInfo.waitSemaphoreCount = static_cast<uint32_t>(info.waitSemaphores.size());
  submitInfo.pWaitSemaphores = info.waitSemaphores.data();
  submitInfo.pWaitDstStageMask = &info.waitStage;
  submitInfo.signalSemaphoreCount = static_cast<uint32_t>(info.signalSemaphores.size());
  submitInfo.pSignalSemaphores = info.signalSemaphores.data();

  if (info.resetFence && info.fence)
    info.fence->reset();
  VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, info.fence ? *info.fence : VK_NULL_HANDLE));
  if (!info.fence)
    VK_CHECK(vkQueueWaitIdle(queue));
  this->setAsSubmitted();
}

CommandBuffer& CommandBuffer::reset(bool releaseResources) {
  if (this->state == State::Ready)
    return *this;
  ASSERT(this->state == State::RecordingEnded || this->state == State::Submitted, "Invalid command buffer state");
  VkCommandBufferResetFlags flags = 0;
  if (releaseResources)
    flags |= VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT;
  VK_CHECK(vkResetCommandBuffer(this->handle, flags));
  this->state = State::Ready;
  return *this;
}

std::vector<CommandBuffer> CommandBuffer::CreateMultiple(
  Device& device,
  VkCommandPool pool,
  uint32_t count,
  bool isPrimary
) {
  std::vector<CommandBuffer> buffers;
  CreateMultiple(buffers, device, pool, count, isPrimary);
  return buffers;
}

void CommandBuffer::CreateMultiple(
  std::vector<CommandBuffer>& buffers,
  Device& device,
  VkCommandPool pool,
  uint32_t count,
  bool isPrimary
) {
  VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
  allocInfo.commandPool = pool;
  allocInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = count;

  std::vector<VkCommandBuffer> handles(count);
  VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, handles.data()));

  buffers.reserve(count);
  for (auto handle : handles) {
    buffers.emplace_back(device, pool, handle, allocInfo.level);
  }
}