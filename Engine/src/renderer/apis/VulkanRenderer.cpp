#include "renderer/apis/VulkanRenderer.h"
#include <core/EngineInfo.h>
#include <GLFW/glfw3.h>

using Engine::VulkanRenderer;

VulkanRenderer::VulkanRenderer(ApplicationInfo& appInfo, Platform& platform)
  : Renderer(appInfo, platform, Renderer::API::Vulkan) {
  this->init();
}

VulkanRenderer::~VulkanRenderer() {
  vkDestroyInstance(this->instance, this->allocator);
  LOG_INFO("Vulkan instance destroyed!");
}

void VulkanRenderer::init() {
  VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
  appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.pApplicationName = this->appInfo.windowInfo.title.data();
  appInfo.applicationVersion = VK_MAKE_API_VERSION(0, this->appInfo.version.major, this->appInfo.version.minor, this->appInfo.version.patch);
  appInfo.pEngineName = "GameEngine";
  appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);

  VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
  createInfo.pApplicationInfo = &appInfo;

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  if (vkCreateInstance(&createInfo, this->allocator, &this->instance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan instance!");
  }
  LOG_INFO("Vulkan instance created!");
}

bool VulkanRenderer::beginFrame() {
  return true;
}

bool VulkanRenderer::endFrame() {
  return true;
}

void VulkanRenderer::onResize(uint32_t width, uint32_t height) {}