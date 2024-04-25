#pragma once

#include "renderer/RendererAPI.h"
#include "Device.h"

#include <core/Application.h>
#include <vulkan/vulkan.h>

namespace Engine::Renderers::Vulkan {
  class Renderer : public RendererAPI {
  public:
    Renderer() = delete;
    Renderer(ApplicationInfo& appInfo, Platform& platform);
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    ~Renderer() override;
    bool beginFrame() override;
    bool endFrame() override;
    void onResize(uint32_t width, uint32_t height) override;
  private:
    void init();
  private:
    Vulkan::Device device;
  };
}