#include "renderer/apis/Vulkan/Semaphore.h"

using namespace Engine::Renderers::Vulkan;

Semaphore::Semaphore(Device& device) : device(device) {
  this->init();
}

Semaphore::~Semaphore() {
  if (this->handle != VK_NULL_HANDLE)
    vkDestroySemaphore(this->device, this->handle, this->device.getAllocator());
}

void Semaphore::init() {
  VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
  VK_CHECK(vkCreateSemaphore(this->device, &createInfo, this->device.getAllocator(), &this->handle));
}
