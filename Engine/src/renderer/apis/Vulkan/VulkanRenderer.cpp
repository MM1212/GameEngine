#include "renderer/apis/Vulkan/VulkanRenderer.h"
#include <core/EngineInfo.h>
#include <GLFW/glfw3.h>

using Engine::Renderers::Vulkan::Renderer;

Renderer::Renderer(ApplicationInfo& appInfo, Platform& platform)
  : RendererAPI(appInfo, platform, RendererAPI::API::Vulkan), device(appInfo, *platform.window) {
  this->init();
}

Renderer::~Renderer() {}

void Renderer::init() {}

bool Renderer::beginFrame() {
  return true;
}

bool Renderer::endFrame() {
  return true;
}

void Renderer::onResize(uint32_t width, uint32_t height) {}