#pragma once

#include "KeyCodes.h"
#include <glm/glm.hpp>
#include <engine/events/Event.h>

namespace Engine {
  class Platform;
}

namespace Engine::Input {
  struct KeyEvent : public Event {
    struct Tag : public EventTag {
      Tag() : EventTag("engine:input:onKeyChange") {}
    };
    KeyCode key;
    bool pressed;
    KeyEvent(KeyCode key, bool pressed) : Event(Tag{}), key(key), pressed(pressed) {}
  };
  struct MouseButtonEvent : public Event {
    struct Tag : public EventTag {
      Tag() : EventTag("engine:input:onMouseButtonChange") {}
    };
    MouseCode button;
    bool pressed;
    MouseButtonEvent(MouseCode button, bool pressed)
      : Event(Tag{}), button(button), pressed(pressed) {}
  };
  struct MouseMoveEvent : public Event {
    struct Tag : public EventTag {
      Tag() : EventTag("engine:input:onMouseMove") {}
    };
    glm::vec2 position;
    glm::vec2 delta;
    MouseMoveEvent(const glm::vec2& position, const glm::vec2& delta)
      : Event(Tag{}), position(position), delta(delta) {}
  };
  struct MouseScrollEvent : public Event {
    struct Tag : public EventTag {
      Tag() : EventTag("engine:input:onMouseScroll") {}
    };
    glm::vec2 offset;
    MouseScrollEvent(const glm::vec2& offset) : Event(Tag{}), offset(offset) {}
  };
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
}