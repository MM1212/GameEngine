#pragma once

#include "defines.h"
#include "Device.h"

#include <unordered_map>
#include <memory>

namespace Engine::Renderers::Vulkan {
  class DescriptorSetLayout {
  public:
    using BindingsMap = std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>;
    class Builder {
    public:
      Builder(Device& device) : device{ device } {}

      Builder& addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1
      );
      std::unique_ptr<DescriptorSetLayout> build() const;

    private:
      Device& device;
      BindingsMap bindings{};
    };

    DescriptorSetLayout(
      Device& device,
      const BindingsMap& bindings
    );
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    operator VkDescriptorSetLayout() const { return handle; }
    VkDescriptorSetLayout getHandle() const { return handle; }
  private:
    Device& device;
    VkDescriptorSetLayout handle;
    BindingsMap bindings;

    friend class DescriptorWriter;
  };

  class DescriptorPool {
  public:
    class Builder {
    public:
      Builder(Device& device) : device{ device } {}

      Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
      inline Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags) { this->poolFlags = flags; return *this; }
      inline Builder& setMaxSets(uint32_t count) { this->maxSets = count; return *this; }
      std::unique_ptr<DescriptorPool> build() const;

    private:
      Device& device;
      std::vector<VkDescriptorPoolSize> poolSizes{};
      uint32_t maxSets = 1000;
      VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    DescriptorPool(
      Device& device,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize>& poolSizes
    );
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

    bool allocSet(
      const VkDescriptorSetLayout descriptorSetLayout,
      VkDescriptorSet& descriptor
    ) const;
    void freeSets(const std::vector<VkDescriptorSet>& descriptors) const;
    void reset();

    operator VkDescriptorPool() const { return handle; }
    VkDescriptorPool getHandle() const { return handle; }
  private:
    Device& device;
    VkDescriptorPool handle;

    friend class DescriptorWriter;
  };

  class DescriptorWriter {
  public:
    DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

    DescriptorWriter& write(uint32_t binding, std::function<void(VkWriteDescriptorSet&)> cb);
    DescriptorWriter& write(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    DescriptorWriter& write(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);
  private:
    DescriptorSetLayout& setLayout;
    DescriptorPool& pool;
    std::vector<VkWriteDescriptorSet> writes;
  };
}