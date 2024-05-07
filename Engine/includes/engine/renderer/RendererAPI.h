#pragma once

#include <cstdint>
#include <memory>

#include "FrameInfo.h"
#include "Texture.h"

#include <engine/platform/Platform.h>
#include <engine/renderer/Camera.h>
#include <engine/scene/components/Transform.h>

namespace Engine {
  struct ApplicationInfo;

  class Renderer {
  public:
    enum class API : uint8_t {
      None = 0,
      Vulkan = 1,
      OpenGL = 2,
      DirectX = 3
    };
    static constexpr API GetApiFromName(const std::string_view& name) {
      if (name == "Vulkan") return API::Vulkan;
      if (name == "OpenGL") return API::OpenGL;
      if (name == "DirectX") return API::DirectX;
      return API::None;
    }
    static constexpr std::string_view GetApiName(API api) {
      switch (api) {
        case API::Vulkan: return "Vulkan";
        case API::OpenGL: return "OpenGL";
        case API::DirectX: return "DirectX";
        default: return "None";
      }
    }
    static constexpr API DEFAULT_API = API::Vulkan;

    Renderer() = delete;
    Renderer(ApplicationInfo& appInfo, Platform& platform, API api);
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    virtual ~Renderer() = default;
    virtual float getAspectRatio() const = 0;
    virtual glm::uvec2 getViewportSize() const = 0;
    virtual bool beginFrame(FrameInfo& frameInfo) = 0;
    virtual bool endFrame(FrameInfo& frameInfo) = 0;
    // Can be manually called to resize the frame buffer but will also be called by event system when window is resized
    virtual void onResize(uint32_t width, uint32_t height) = 0;

    virtual Ref<Texture2D> createTexture2D(const TextureSpecification& spec) = 0;
    virtual Ref<Texture2D> createTexture2D(const std::string_view& path) = 0;

    static Ref<spdlog::logger>& GetLogger() { return Logger; }
    static Scope<Renderer> Create(ApplicationInfo& appInfo, Platform& platform, API api = DEFAULT_API);
    static API GetAPI() { return instance->api; }
    static Renderer* Get() { return instance; }
  protected:
    static Ref<spdlog::logger> Logger;
    ApplicationInfo& appInfo;
    Platform& platform;
    API api;
    static Renderer* instance;
  };
}