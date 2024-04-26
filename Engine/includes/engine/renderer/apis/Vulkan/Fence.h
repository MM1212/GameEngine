#pragma once

#include "defines.h"
#include "Device.h"

namespace Engine::Renderers::Vulkan {
  class Fence {
  public:
    Fence(Device& device, bool signaled = false);
    ~Fence();

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;

    Fence(Fence&& other) = default;
    Fence& operator=(Fence&& other) = default;

    operator bool() const {
      return this->handle != VK_NULL_HANDLE;
    }
    operator VkFence() const {
      return this->handle;
    }
    VkFence getHandle() const { return this->handle; }

    void reset();
    bool wait(uint64_t timeout = UINT64_MAX);
  private:
    void init(bool signaled);
  private:
    Device& device;
    VkFence handle = VK_NULL_HANDLE;
  };
}