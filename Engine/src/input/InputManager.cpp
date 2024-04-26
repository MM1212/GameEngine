#include <engine/platform/Platform.h>
#include "engine/input/InputManager.h"
#include <GLFW/glfw3.h>
#include <events/EventSystem.h>

using namespace Engine::Input;

static inline InputManager* GetManager(GLFWwindow* window) {
  static Engine::Platform* platform = reinterpret_cast<Engine::Platform*>(glfwGetWindowUserPointer(window));
  return platform->input;
}

void InputManager::GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)scancode;
  (void)mods;
  auto manager = GetManager(window);

  if (manager->keyboard.onEvent(key, action != GLFW_RELEASE))
    EventSystem::Get()->emit<KeyEvent>(key, action != GLFW_RELEASE);
}

void InputManager::GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  (void)mods;
  auto manager = GetManager(window);

  if (manager->mouseButtons.onEvent(button, action != GLFW_RELEASE))
    EventSystem::Get()->emit<MouseButtonEvent>(button, action != GLFW_RELEASE);
}

void InputManager::GlfwCursorPosCallback(GLFWwindow* window, double x, double y) {
  auto manager = GetManager(window);
  glm::vec2 position = { x, y };
  if (position == manager->mouse.position) return;
  manager->mouse.lastPosition = manager->mouse.position;
  manager->mouse.position = { x, y };
  manager->mouse.delta = manager->mouse.position - manager->mouse.lastPosition;
  EventSystem::Get()->emit<MouseMoveEvent>(manager->mouse.position, manager->mouse.delta);
}

void InputManager::GlfwScrollCallback(GLFWwindow* window, double x, double y) {
  auto manager = GetManager(window);

  // some mouse wheels report random offsets, so we clamp them to -1, 0, 1
  x = x < 0 ? -1 : x > 0 ? 1 : 0;
  y = y < 0 ? -1 : y > 0 ? 1 : 0;
  if (x == 0 && y == 0) return;
  manager->mouse.scrollOffset = { x, y };
  EventSystem::Get()->emit<MouseScrollEvent>(manager->mouse.scrollOffset);
}

InputManager::InputManager(Platform& platform) : platform(platform) {
#ifdef _DEBUG
  EventSystem::Get()->on<KeyEvent>([](KeyEvent& event) -> bool {
    LOG_TRACE("Key: {}[{}] {}", static_cast<char>(event.key), event.key, event.pressed ? "pressed" : "released");
    return false;
    });
  EventSystem::Get()->on<MouseButtonEvent>([](MouseButtonEvent& event) -> bool {
    LOG_TRACE("Mouse Button: {} {}", event.button, event.pressed ? "pressed" : "released");
    return false;
    });
#if 0
  EventSystem::Get()->on<MouseMoveEvent>([](MouseMoveEvent& event) -> bool {
    LOG_TRACE("Mouse Position: ({}, {}) Delta: ({}, {})", event.position.x, event.position.y, event.delta.x, event.delta.y);
    return false;
    });
  EventSystem::Get()->on<MouseScrollEvent>([](MouseScrollEvent& event) -> bool {
    LOG_TRACE("Mouse Scroll: ({}, {})", event.offset.x, event.offset.y);
    return false;
    });
#endif
#endif
}

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