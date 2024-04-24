#pragma once

#include "KeyCodes.h"
#include <glm/glm.hpp>

namespace Engine {
  class Platform;
}

namespace Engine::Input {
  bool IsKeyPressed(KeyCode key);
  bool IsKeyDown(KeyCode key);
  bool IsKeyUp(KeyCode key);
  enum class MouseMode {
    Normal,
    Hidden,
    Disabled
  };
  bool IsMouseButtonPressed(MouseCode button);
  bool IsMouseButtonDown(MouseCode button);
  bool IsMouseButtonUp(MouseCode button);
  glm::vec2 GetMousePosition();
  float GetMouseX();
  float GetMouseY();
  glm::vec2 GetMouseDelta();
  void SetMouseMode(MouseMode mode);
  MouseMode GetMouseMode();
  void SetMousePosition(const glm::vec2& position);
  void Update();
  void Init(Platform& platform);
}