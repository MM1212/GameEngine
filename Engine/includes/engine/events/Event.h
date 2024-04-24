#pragma once

#include <string_view>
#include <iostream>
#include <engine/utils/hash.h>

namespace Engine {
  struct EventTag {
    using ID = uint64_t;
    using Tag = std::string_view;
    ID id = 0;
    Tag name = nullptr;

    EventTag() = delete;
    EventTag(ID id) : id(id) {}
    constexpr EventTag(Tag name) : name(name), id(Hash(name)) {}
    EventTag(const EventTag& other) = default;
    EventTag(EventTag&& other) = default;
    EventTag& operator=(const EventTag& other) = default;
    explicit operator ID() const { return this->id; }
    explicit operator Tag() const { return this->name; }
    bool operator==(const EventTag& other) const { return this->id == other.id; }
    bool operator!=(const EventTag& other) const { return this->id != other.id; }
  };
  struct Event {
    Event(EventTag tag) : tag(tag) {}
    virtual ~Event() = default;

    explicit operator EventTag() const { return this->tag; }
    explicit operator EventTag::ID() const { return this->tag.id; }
    explicit operator EventTag::Tag() const { return this->tag.name; }
  
    EventTag tag;
    bool handled = false;
  };

  template<typename OStream>
  inline OStream& operator<<(OStream& os, const Event& event)
  {
    return os << static_cast<EventTag::Tag>(event);
  }

  template <typename T>
  class EventDispatcher {
  public:
    EventDispatcher(T& event) : event(event) {}
    template <typename F>
    void dispatch(F&& func) {
      this->event.handled |= func(this->event);
    }
  private:
    T& event;
  };
}