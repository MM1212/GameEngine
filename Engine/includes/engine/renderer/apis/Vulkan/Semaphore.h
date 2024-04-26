#pragma once

#include "defines.h"
#include "Device.h"

namespace Engine::Renderers::Vulkan {
  class Semaphore {
  public:
    Semaphore(Device& device);
    ~Semaphore();

    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore(Semaphore&& other) = default;
    Semaphore& operator=(Semaphore&& other) = default;

    operator bool() const {
      return this->handle != VK_NULL_HANDLE;
    }
    operator VkSemaphore() const {
      return this->handle;
    }
    VkSemaphore getHandle() const { return this->handle; }

  private:
    void init();
  private:
    Device& device;
    VkSemaphore handle = VK_NULL_HANDLE;
  };
}