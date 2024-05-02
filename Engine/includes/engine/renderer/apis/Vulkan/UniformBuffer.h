#pragma once

#include "defines.h"
#include "MemBuffer.h"
#include "Device.h"
#include "Descriptors.h"

namespace Engine::Renderers::Vulkan {
  template <typename T>
  class UniformBuffer {
  public:
    template <typename ...Args>
    UniformBuffer(
      Device& device,
      uint32_t frames,
      Args&&... args
    ) : device(device), frames(frames), data(args...) {
      this->alloc();
    }
    ~UniformBuffer() = default;

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) = default;
    UniformBuffer& operator=(UniformBuffer&&) = default;

    void update(uint32_t frameIndex) {
      this->buffer->writeToIndex(&this->data, frameIndex);
      this->buffer->flushIndex(frameIndex);
    }

    bool bind(
      DescriptorPool& pool,
      DescriptorSetLayout& layout,
      uint32_t frameIndex,
      uint32_t binding,
      VkDescriptorSet& outSet
    ) {
      auto bufferInfo = this->buffer->descriptorInfoForIndex(frameIndex);
      return DescriptorWriter(layout, pool)
        .write(binding, &bufferInfo)
        .build(outSet);
    }

    operator T& () {
      return this->data;
    }
    operator T& () const {
      return this->data;
    }

    MemBuffer& getBuffer() const {
      return *this->buffer;
    }

  private:
    void alloc() {
      this->buffer = std::make_unique<MemBuffer>(
        this->device,
        sizeof(T),
        this->frames,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
      );
      this->buffer->map();
    }
    Device& device;
    uint32_t frames;
    T data;
    Scope<MemBuffer> buffer;
  };
}