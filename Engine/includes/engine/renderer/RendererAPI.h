#pragma once

#include <cstdint>
#include <memory>

#include <engine/platform/Platform.h>

namespace Engine {
  struct ApplicationInfo;

  struct GlobalUboObject {
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 viewProjection;
  };

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
    virtual bool beginFrame() = 0;
    virtual bool endFrame() = 0;
    // Can be manually called to resize the frame buffer but will also be called by event system when window is resized
    virtual void onResize(uint32_t width, uint32_t height) = 0;

    static std::shared_ptr<spdlog::logger>& GetLogger() { return Logger; }
    static std::unique_ptr<Renderer> Create(ApplicationInfo& appInfo, Platform& platform, API api = DEFAULT_API);
  protected:
    static std::shared_ptr<spdlog::logger> Logger;
    ApplicationInfo& appInfo;
    Platform& platform;
    API api;
  };
}