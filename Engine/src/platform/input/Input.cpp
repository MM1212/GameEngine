#include "engine/platform/input/Input.h"
#include <engine/platform/input/InputManager.h>
#include <engine/core/Application.h>

#include <unordered_map>
#include <GLFW/glfw3.h>

namespace Engine::Input {
  static inline InputManager& GetManager() {
    return Engine::Application::Get()->getInputManager();
  }

  bool IsKeyPressed(KeyCode key) {
    return GetManager().isKeyPressed(key);
  }
  bool IsKeyDown(KeyCode key) {
    return GetManager().isKeyJustPressed(key);
  }
  bool IsKeyUp(KeyCode key) {
    return GetManager().isKeyJustReleased(key);
  }
  bool IsMouseButtonPressed(MouseCode button) {
    return GetManager().isMouseButtonPressed(button);
  }
  bool IsMouseButtonDown(MouseCode button) {
    return GetManager().isMouseButtonPressed(button);
  }
  bool IsMouseButtonUp(MouseCode button) {
    return GetManager().isMouseButtonPressed(button);
  }
  glm::vec2 GetMousePosition() {
    return GetManager().getMousePosition();
  }
  float GetMouseX() {
    return GetManager().getMouseX();
  }
  float GetMouseY() {
    return GetManager().getMouseY();
  }
  glm::vec2 GetMouseDelta() {
    return GetManager().getMouseDelta();
  }
  void SetMouseMode(MouseMode mode) {
    GetManager().setMouseMode(mode);
  }
}
