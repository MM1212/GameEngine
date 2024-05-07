#include "renderer/apis/Vulkan/Fence.h"
#include "renderer/apis/Vulkan/Device.h"
#include <renderer/logger.h>

using namespace Engine::Renderers::Vulkan;

Fence::Fence(Device& device, bool signaled) : device(device) {
  this->init(signaled);
}

Fence::~Fence() {
  if (this->handle != VK_NULL_HANDLE)
    vkDestroyFence(this->device, this->handle, this->device.getAllocator());
}

void Fence::init(bool signaled) {
  VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
  createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
  VK_CHECK(vkCreateFence(this->device, &createInfo, this->device.getAllocator(), &this->handle));
}

void Fence::reset() {
  VK_CHECK(vkResetFences(this->device, 1, &this->handle));
}

bool Fence::wait(uint64_t timeout) {
  auto result = vkWaitForFences(this->device, 1, &this->handle, VK_TRUE, timeout);
  switch (result) {
    case VK_SUCCESS:
      return true;
    case VK_TIMEOUT:
      LOG_RENDERER_WARN("Fence::wait() - Timed out");
      break;
    case VK_ERROR_DEVICE_LOST:
      LOG_RENDERER_ERROR("Fence::wait() - Device lost");
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      LOG_RENDERER_ERROR("Fence::wait() - Out of host memory");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      LOG_RENDERER_ERROR("Fence::wait() - Out of device memory");
      break;
    default:
      LOG_RENDERER_ERROR("Fence::wait() - Unknown error");
      break;
  }

  return false;
}