#pragma once

#include <memory>
#include <engine/utils/logger.h>
#include <engine/utils/asserts.h>
#include <engine/events/Event.h>

#include <glm/glm.hpp>

namespace Engine {
  class Platform;
  struct WindowSpecs {
    union {
      glm::uvec2 size;
      struct {
        uint32_t width;
        uint32_t height;
      };
    };
    std::string_view title;
    bool vSync = false;
    bool resizable = true;
  };
  struct WindowCloseEvent : public Event {
    struct Tag : public EventTag {
      Tag() : EventTag{ "engine:window:onClose" } {}
    };
    WindowCloseEvent() : Event(Tag{}) {}
    ~WindowCloseEvent() = default;
  };
  struct WindowResizeEvent : public Event {
    struct Tag : public EventTag {
      Tag() : EventTag{ "engine:window:onResize" } {}
    };
    WindowResizeEvent(uint32_t width, uint32_t height) : Event(Tag{}), width(width), height(height) {}
    WindowResizeEvent(glm::uvec2 size) : Event(Tag{}), width(size.x), height(size.y) {}
    ~WindowResizeEvent() = default;
    uint32_t width;
    uint32_t height;
  };
  class Window {
  public:
    Window(Platform& platform, WindowSpecs specs);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    const void* getHandle() const { return this->handle; }
    void* getHandle() { return this->handle; }
    const glm::uvec2& getSize() const { return this->size; }
    uint32_t getWidth() const { return this->size.x; }
    uint32_t getHeight() const { return this->size.y; }
    bool wasResized() const { return this->resized; }
    bool shouldClose() const;
    void close();
    void pollEvents();
    void resetResizedFlag() { this->resized = false; }

    void init();
    // TODO: window surface
  private:
    Platform& platform;
    WindowSpecs spec;
    glm::uvec2 size;
    bool resized = false;
    const std::string_view title;
    void* handle = nullptr;
  };
}