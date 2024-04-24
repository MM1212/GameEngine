#pragma once

#include <string_view>
#include "Event.h"

#include <queue>
#include <unordered_map>
#include <any>
#include <functional>
#include <engine/utils/logger.h>

namespace std {
  template <>
  constexpr bool greater<Engine::EventSystem::EventListener>::operator()(
    const Engine::EventSystem::EventListener& a,
    const Engine::EventSystem::EventListener& b
    ) const {
    return a.priority < b.priority;
  }
}

namespace Engine {
  class EventSystem {
  public:
    struct EventListenerHandle {
      EventTag tag;
      uint32_t handle;

      EventListenerHandle() = default;
      EventListenerHandle(EventTag tag, uint32_t handle) : tag(tag), handle(handle) {}
      EventListenerHandle(const EventListenerHandle&) = default;
      EventListenerHandle(EventListenerHandle&&) = default;
      EventListenerHandle& operator=(const EventListenerHandle&) = default;
    };
  private:
    struct EventListener {
      std::function<bool(Event&)> listener;
      uint8_t priority = 0;
      uint32_t handle = 0;
    };
    using Container = std::priority_queue<EventListener, std::vector<EventListener>, std::greater<EventListener>>;
    class EventListenerQueue : public Container {
    public:
      template <typename T>
      bool operator()(T& event) {
        bool handled = false;
        for (auto& listener : this->c) {
          try {
            handled |= dynamic_cast<std::function<bool(T&)>&>(listener.listener)(event);
            if (event.handled) {
              break;
            }
          }
          catch (const std::bad_cast& e) {
            LOG_ERROR("Event listener threw an exception: {}", e.what());
          }
        }
        return handled;
      }
      bool pop(const EventListenerHandle& handle) {
        auto it = std::find_if(this->c.begin(), this->c.end(), [&handle](const EventListener& listener) {
          return listener.handle == handle.handle;
          });
        if (it != this->c.end()) {
          this->c.erase(it);
          return true;
        }
        return false;
      }
    };
  public:
    static EventSystem* Get() { return instance; }
    EventSystem();
    ~EventSystem() = default;

    template <typename T>
    EventListenerHandle bind(EventTag tag, T&& listener, uint8_t priority = 0) {
      uint32_t handle = this->listenerId++;
      this->listeners[static_cast<EventTag::ID>(tag)].push(EventListener{ std::forward<T>(listener), priority, handle });
      return { tag, handle };
    }

    bool unbind(const EventListenerHandle& handle) {
      if (this->listeners.count(static_cast<EventTag::ID>(handle.tag))) {
        return this->listeners[static_cast<EventTag::ID>(handle.tag)].pop(handle);
      }
    }

    template <typename T>
    bool dispatch(T& event) {
      if (this->listeners.count(static_cast<EventTag::ID>(event))) {
        return this->listeners[static_cast<EventTag::ID>(event)](event);
      }
      return false;
    }

  private:
    std::unordered_map<EventTag::ID, EventListenerQueue> listeners;
    uint32_t listenerId = 0;
    static EventSystem* instance;
  };
}