#include "renderer/RendererAPI.h"
#include <core/Application.h>
#include <renderer/apis/Vulkan/VulkanRenderer.h>
#include <events/EventSystem.h>
#include <platform/Window.h>

using Engine::RendererAPI;

RendererAPI::RendererAPI(ApplicationInfo& appInfo, Platform& platform, API api)
  : appInfo(appInfo), platform(platform), api(api) {
  EventSystem::Get()->on<WindowResizeEvent>([this](WindowResizeEvent& event) -> bool {
    this->onResize(event.width, event.height);
    return false;
  }, 100);
}

std::unique_ptr<RendererAPI> RendererAPI::Create(ApplicationInfo& appInfo, Platform& platform, API api) {
  try {
    switch (api) {
    case API::Vulkan:
      return std::make_unique<Renderers::Vulkan::Renderer>(appInfo, platform);
    case API::OpenGL:
      LOG_CRITICAL("OpenGL is not supported yet!");
      // return std::make_unique<OpenGLRenderer>(appInfo, platform);
    case API::DirectX:
      LOG_CRITICAL("DirectX is not supported yet!");
      // return std::make_unique<DirectXRenderer>(appInfo, platform);
    default:
      return nullptr;
    }
  }
  catch (const std::exception& e) {
    LOG_CRITICAL("Failed to create {} renderer: {}", RendererAPI::GetApiName(api), e.what());
    return nullptr;
  }
}