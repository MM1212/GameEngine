#include <engine/platform/Platform.h>
#include "engine/input/InputManager.h"
#include <GLFW/glfw3.h>

using namespace Engine::Input;

static inline InputManager* GetManager(GLFWwindow* window) {
  static Engine::Platform* platform = reinterpret_cast<Engine::Platform*>(glfwGetWindowUserPointer(window));
  return platform->input;
}

void InputManager::GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;
  auto manager = GetManager(window);
  manager->keyboard.onEvent(key, action != GLFW_RELEASE);
}

void InputManager::GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  (void)mods;
  auto manager = GetManager(window);
  manager->mouseButtons.onEvent(button, action != GLFW_RELEASE);
}

void InputManager::GlfwCursorPosCallback(GLFWwindow* window, double x, double y) {
  auto manager = GetManager(window);
  manager->mouse.lastPosition = manager->mouse.position;
  manager->mouse.position = { x, y };
  manager->mouse.delta = manager->mouse.position - manager->mouse.lastPosition;
}

void InputManager::GlfwScrollCallback(GLFWwindow* window, double x, double y) {
  auto manager = GetManager(window);
  manager->mouse.scrollOffset = { x, y };
}

InputManager::InputManager(Platform& platform) : platform(platform) {}

GLFWwindow* InputManager::getWindowHandle() {
  return reinterpret_cast<GLFWwindow*>(this->platform.window->getHandle());
}

void InputManager::init() {
  this->bindCallbacks(this->getWindowHandle());
}

void InputManager::update() {
  this->keyboard.update();
  this->mouseButtons.update();
  this->mouse.delta = { 0.0f, 0.0f };
  this->mouse.scrollOffset = { 0.0f, 0.0f };
}

void InputManager::bindCallbacks(GLFWwindow* window) {
  glfwSetKeyCallback(window, GlfwKeyCallback);
  glfwSetMouseButtonCallback(window, GlfwMouseButtonCallback);
  glfwSetCursorPosCallback(window, GlfwCursorPosCallback);
  glfwSetScrollCallback(window, GlfwScrollCallback);
}

static std::unordered_map<MouseMode, uint32_t> MouseModeMap = {
  { MouseMode::Normal, GLFW_CURSOR_NORMAL },
  { MouseMode::Hidden, GLFW_CURSOR_HIDDEN },
  { MouseMode::Disabled, GLFW_CURSOR_DISABLED }
};

void InputManager::setMouseMode(MouseMode mode) {
  glfwSetInputMode(this->getWindowHandle(), GLFW_CURSOR, static_cast<int>(MouseModeMap[mode]));
  this->mouse.mode = mode;
}
void InputManager::setMousePosition(glm::vec2 position) {
  glfwSetCursorPos(this->getWindowHandle(), position.x, position.y);
  this->mouse.position = position;
}