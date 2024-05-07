#pragma once

#include "defines.h"
#include "Device.h"
#include "Image.h"

#include <engine/renderer/Texture.h>
#include <engine/utils/glm.h>

namespace Engine::Renderers::Vulkan {

  class Texture2D : public Engine::Texture2D, protected Size2D<uint32_t> {
  public:
    Texture2D(Device& device, const TextureSpecification& spec);
    ~Texture2D() override;

    const TextureSpecification& getSpecification() const override { return this->spec; }

    const glm::uvec2& getSize() const override { return this->size; }
    uint32_t getWidth() const override { return this->width; }
    uint32_t getHeight() const override { return this->height; }
    std::string_view getPath() const override { return this->path; }
    uint64_t getId() const override { return this->id; }
    bool isLoaded() const override { return this->loaded; }

    void bind(uint32_t slot = 0) const override;
    void loadData(const void* data, uint32_t size) override;


    bool operator==(const Texture& other) const override { return this->id == other.getId(); }

    static VkFormat TexChannelsToVkFormat(TextureChannels channels);
    static VkSamplerCreateInfo CreateSamplerInfo(const TextureSamplerSpecification& spec);
  private:
    void init();
  private:
    Device& device;
    TextureSpecification spec;
    std::string path;

    uint32_t generation = 0;
    Scope<Image> image = nullptr;
    VkSampler sampler = VK_NULL_HANDLE;
  };
}