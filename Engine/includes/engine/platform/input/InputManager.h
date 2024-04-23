#pragma once

#include "Input.h"
#include <engine/utils/asserts.h>

#include <array>
#include <unordered_set>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace Engine {
  class Platform;
}

namespace Engine::Input {
  class InputManager {
  private:
    template <typename T, uint32_t M>
    using KeyCodeMap = std::array<T, M>;
    template <typename T, uint32_t M, typename C>
    class KeyStateMap {
    public:
      KeyStateMap() = default;
      KeyStateMap(const KeyStateMap&) = delete;
      KeyStateMap& operator=(const KeyStateMap&) = delete;

      bool isPressed(C key) const {
        ASSERT(key < M, "Key out of range!");
        auto& state = this->getState(key);
        return state.pressed;
      }
      bool isJustPressed(C key) const {
        ASSERT(key < M, "Key out of range!");
        auto& state = this->getState(key);
        return !state.wasPressed && state.pressed;
      }
      bool isJustReleased(C key) const {
        ASSERT(key < M, "Key out of range!");
        auto& state = this->getState(key);
        return state.wasPressed && !state.pressed;
      }
      bool isHeld(C key) const {
        ASSERT(key < M, "Key out of range!");
        auto& state = this->getState(key);
        return state.wasPressed && state.pressed;
      }
      bool isReleased(C key) const {
        return !this->isPressed(key);
      }
      void onEvent(C key, bool pressed) {
        ASSERT(key < M, "Key out of range!");
        auto& state = this->getState(key);
        state.wasPressed = state.pressed;
        state.pressed = pressed;
        this->toUpdate.insert(key);
      }
      void update() {
        for (auto key : this->toUpdate) {
          auto& state = this->getState(key);
          state.wasPressed = state.pressed;
        }
        this->toUpdate.clear();
      }
    private:
      const T& getState(C key) const { return this->state[key]; }
      T& getState(C key) { return this->state[key]; }
      KeyCodeMap<T, M> state{};
      std::unordered_set<C> toUpdate{};
    };
  public:
    InputManager(Platform& platform);
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    void init();

    bool isKeyPressed(KeyCode key) const {
      return this->keyboard.isPressed(key);
    }
    bool isKeyReleased(KeyCode key) const {
      return this->keyboard.isReleased(key);
    }
    bool isKeyJustPressed(KeyCode key) const {
      return this->keyboard.isJustPressed(key);
    }
    bool isKeyJustReleased(KeyCode key) const {
      return this->keyboard.isJustReleased(key);
    }
    bool isKeyHeld(KeyCode key) const {
      return this->keyboard.isHeld(key);
    }
    bool isMouseButtonPressed(MouseCode button) const {
      return this->mouseButtons.isPressed(button);
    }
    bool isMouseButtonReleased(MouseCode button) const {
      return this->mouseButtons.isReleased(button);
    }
    bool isMouseButtonJustPressed(MouseCode button) const {
      return this->mouseButtons.isJustPressed(button);
    }
    bool isMouseButtonJustReleased(MouseCode button) const {
      return this->mouseButtons.isJustReleased(button);
    }
    bool isMouseButtonDown(MouseCode button) const {
      return this->mouseButtons.isHeld(button);
    }
    glm::vec2 getMousePosition() const {
      return this->mouse.position;
    }
    float getMouseX() const {
      return this->mouse.position.x;
    }
    float getMouseY() const {
      return this->mouse.position.y;
    }
    glm::vec2 getMouseDelta() const {
      return this->mouse.delta;
    }
    glm::vec2 getScrollOffset() const {
      return this->mouse.scrollOffset;
    }
    MouseMode getMouseMode() const {
      return this->mouse.mode;
    }
    void setMouseMode(MouseMode mode);
    void setMousePosition(glm::vec2 position);
    float getScrollY() const {
      return this->mouse.scrollOffset.y;
    }
    float getScrollX() const {
      return this->mouse.scrollOffset.x;
    }
    void update();
  private:
    GLFWwindow* getWindowHandle();
    void handleKeyInput(uint16_t key, int action);
    void bindCallbacks(GLFWwindow* window);
    static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void GlfwCursorPosCallback(GLFWwindow* window, double x, double y);
    static void GlfwScrollCallback(GLFWwindow* window, double x, double y);
    struct KeyState {
      bool pressed = false;
      bool wasPressed = false;
    };
    struct MouseData {
      glm::vec2 position = { 0.0f, 0.0f };
      glm::vec2 delta = { 0.0f, 0.0f };
      glm::vec2 lastPosition = { 0.0f, 0.0f };
      glm::vec2 scrollOffset = { 0.0f, 0.0f };
      MouseMode mode = MouseMode::Normal;
    };
  private:
    Platform& platform;
    KeyStateMap<KeyState, GLFW_KEY_LAST, KeyCode> keyboard{};
    KeyStateMap<KeyState, GLFW_MOUSE_BUTTON_LAST, KeyCode> mouseButtons{};
    MouseData mouse{};
  };
};