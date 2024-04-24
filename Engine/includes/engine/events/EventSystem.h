#pragma once

#include <string_view>
#include "Event.h"

#include <queue>
#include <unordered_map>
#include <any>
#include <functional>
#include <memory>
#include <engine/utils/logger.h>

namespace Engine {
  class EventSystem {
  public:
    struct EventListenerHandle {
      EventTag tag;
      uint32_t handle;

      EventListenerHandle() = delete;
      EventListenerHandle(EventTag tag, uint32_t handle) : tag(tag), handle(handle) {}
      EventListenerHandle(const EventListenerHandle&) = default;
      EventListenerHandle(EventListenerHandle&&) = default;
      EventListenerHandle& operator=(const EventListenerHandle&) = default;
    };
  private:
    using EventListenerCallback = std::function<bool(Event&)>;
    struct EventListener {
      EventListenerCallback listener;
      uint8_t priority = 0;
      uint32_t handle = 0;

      bool operator>(const EventListener& other) const {
        return this->priority > other.priority;
      }
      bool operator<(const EventListener& other) const {
        return this->priority < other.priority;
      }

      EventListener() = default;
      EventListener(EventListenerCallback listener, uint8_t priority, uint32_t handle)
        : listener(listener), priority(priority), handle(handle) {}
    };
    using Container = std::priority_queue<EventListener, std::vector<EventListener>, std::greater<EventListener>>;
    class EventListenerQueue : public Container {
    public:
      bool operator()(Event& event) {
        bool handled = false;
        for (auto& listener : this->c) {
          try {
            handled |= listener.listener(event);
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


    template <typename T, typename F>
    EventListenerHandle bind(EventTag tag, F cb, uint8_t priority = 0) {
      uint32_t handle = this->listenerId++;
      this->listeners[static_cast<EventTag::ID>(tag)].emplace([cb](Event& raw) {
        T& event = dynamic_cast<T&>(raw);
        return cb(event);
      }, priority, handle);
      return { tag, handle };
    }
    template <typename T, typename F>
    EventListenerHandle bind(F cb, uint8_t priority = 0) {
      auto tag = typename T::Tag{};
      return this->bind<T, F>(tag, cb, priority);
    }
    template <typename T, typename F>
    EventListenerHandle bind(std::string_view eventName, F cb, uint8_t priority = 0) {
      EventTag tag{ eventName };
      return this->bind<T, F>(tag, cb, priority);
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