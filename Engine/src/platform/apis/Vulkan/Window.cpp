#include <renderer/apis/Vulkan/Device.h>
#include <platform/Window.h>
#include <GLFW/glfw3.h>

using Engine::Window;

int Window::createVulkanSurface(void* ptr) {
  auto* device = reinterpret_cast<Renderers::Vulkan::Device*>(ptr);
  return glfwCreateWindowSurface(
    device->getInstance(),
    reinterpret_cast<GLFWwindow*>(this->getHandle()),
    device->getAllocator(),
    &device->surface
  );
}

std::vector<std::string_view> Window::getVulkanRequiredExtensions() {
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  ASSERT(glfwExtensions, "Failed to get GLFW required extensions!");
  std::vector<std::string_view> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  return extensions;
}