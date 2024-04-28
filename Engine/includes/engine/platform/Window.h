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
    WindowResizeEvent() : Event(Tag{}), width(0), height(0) {}
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
    bool shouldClose() const;
    void close();
    void pollEvents();
    void waitEvents();
    void init();
    // Vulkan specific API Calls (defined in platform/apis/Vulkan/Window.cpp)
    int createVulkanSurface(void* device);
    std::vector<std::string_view> getVulkanRequiredExtensions();
  private:
    Platform& platform;
    WindowSpecs spec;
    glm::uvec2 size;
    const std::string_view title;
    void* handle = nullptr;
  };
}