#pragma once

#include "Window.h"

#include <memory>
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Engine {
  namespace Input {
    class InputManager;
  }
  class Platform {
  public:
    Platform(const WindowSpecs specs);
    Platform(const glm::uvec2 size, const std::string_view title);
    Platform(uint32_t width, uint32_t height, const std::string_view title);
    ~Platform();
    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;

    bool init();
    bool shouldExit() const { return this->window->shouldClose(); }
    void update();
  public:
    Window* window = nullptr;
    Input::InputManager* input = nullptr;
  private:
    void setup(const WindowSpecs spec);
    friend class Window;
    friend class Input::InputManager;
  };
}