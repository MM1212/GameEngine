#include "renderer/apis/Vulkan/CommandBuffer.h"

using namespace Engine::Renderers::Vulkan;

DiscardableCommandBuffer::DiscardableCommandBuffer(
  Device& device,
  VkCommandPool pool,
  VkQueue queue,
  CommandBufferSubmitInfo info,
  bool isPrimary
) : CommandBuffer(device, pool, isPrimary), submitQueue(queue), submitInfo(info) {
  this->beginRecording(true, false, false);
}

DiscardableCommandBuffer::~DiscardableCommandBuffer() {
  if (this->state != State::Recording)
    return;
  this->endRecording();
}

void DiscardableCommandBuffer::endRecording() {
  this->CommandBuffer::endRecording();
  this->submit(this->submitQueue, this->submitInfo);
  this->free();
}