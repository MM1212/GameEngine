#include "engine/platform/Window.h"

#include <engine/platform/Platform.h>
#include <stdexcept>
#include <glfw/glfw3.h>
#include <events/EventSystem.h>

using Engine::Window;

#define GLFW_WINDOW(handle) reinterpret_cast<GLFWwindow*>(handle)

Window::Window(
  Platform& platform,
  WindowSpecs specs
) : platform(platform), spec(specs), size(specs.size), title(specs.title) {}

Window::~Window() {
  if (!this->handle)
    return;
  glfwDestroyWindow(GLFW_WINDOW(this->handle));
  glfwTerminate();
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(GLFW_WINDOW(this->handle));
}

void Window::close() {
  glfwSetWindowShouldClose(GLFW_WINDOW(this->handle), GLFW_TRUE);
}

void Window::pollEvents() {
  glfwPollEvents();
}

void Window::waitEvents() {
  glfwWaitEvents();
}

void Window::init() {
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, this->spec.resizable);
  glfwSwapInterval(static_cast<int>(this->spec.vSync));

  this->handle = glfwCreateWindow(
    this->size.x, this->size.y,
    this->title.data(), nullptr, nullptr
  );
  if (!this->handle)
    throw std::runtime_error("Failed to create window");
  glfwSetWindowUserPointer(GLFW_WINDOW(this->handle), &this->platform);
  glfwSetFramebufferSizeCallback(GLFW_WINDOW(this->handle), [](GLFWwindow* handle, int width, int height) {
    auto platform = reinterpret_cast<Platform*>(glfwGetWindowUserPointer(GLFW_WINDOW(handle)));
    if (!platform)
      return;
    glm::uvec2 newSize = { width, height };
    if (platform->window->size == newSize)
      return;
    platform->window->size = newSize;
    EventSystem::Get()->queueUnique<WindowResizeEvent>(platform->window->size);
  });
  glfwSetWindowCloseCallback(GLFW_WINDOW(this->handle), [](GLFWwindow* handle) {
    EventSystem::Get()->queue<WindowCloseEvent>();
  });
}
