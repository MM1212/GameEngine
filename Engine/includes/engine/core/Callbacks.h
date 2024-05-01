#pragma once

#include <functional>
#include <atomic>
#include <queue>
#include <vector>
#include <algorithm>

namespace Engine {
  template <typename... Args>
  class Callback {
  public:
    using Fn = std::function<bool(Args...)>;
  private:
    struct Data {
      Fn function;
      uint8_t priority = 0;
      uint64_t handle = -1;
      Data(Fn function) : function(function) {}

      bool operator>(const Data& other) const {
        return this->priority < other.priority;
      }
      bool operator<(const Data& other) const {
        return this->priority > other.priority;
      }
    };

    class Queue : public std::priority_queue<Data, std::vector<Data>, std::greater<Data>> {
    public:
      bool operator()(Args... args) {
        for (auto& listener : this->c) {
          if (!std::invoke(listener.function, args...)) {
            return false;
          }
        }
        return true;
      }
      bool pop(uint64_t handle) {
        auto it = std::find_if(this->c.begin(), this->c.end(), [handle](const Data& d) {
          return d.handle == handle;
        });
        if (it != this->c.end()) {
          this->c.erase(it);
          return true;
        }
        return false;
      }
      bool empty() const {
        return this->c.empty();
      }
    };

    Queue queue;
    std::atomic<uint64_t> handleIdx = 0;
  public:
    Callback() = default;
    ~Callback() = default;

    template <typename T>
    auto connect(T function) {
      return this->_connect(function, 0);
    }

    template <typename T>
    auto connect(T function, uint8_t priority) {
      if constexpr (std::is_same_v<std::invoke_result_t<T, Args...>, bool>) {
        return this->_connect(function, priority);
      }
      else {
        return this->_connect([function](Args&&... args) {
          std::invoke(function, args...);
          return true;
        }, priority);
      }
    }

    void disconnect(uint64_t handle) {
      if (handle == -1) return;
      this->queue.pop(handle);
    }

    bool operator()(Args... args) {
      return this->queue(args...);
    }
  private:
    template <typename T>
    uint64_t _connect(T function, uint8_t prio) {
      auto handle = this->handleIdx++;
      Data connector(function);
      connector.priority = prio;
      connector.handle = handle;
      this->queue.push(connector);
      return handle;
    }
  };
}