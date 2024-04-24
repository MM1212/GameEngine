#include "events/EventSystem.h"

using Engine::EventSystem;

EventSystem* EventSystem::instance = nullptr;

EventSystem::EventSystem() {
  if (instance) {
    LOG_ERROR("EventSystem already exists");
    return;
  }
  instance = this;
}

EventSystem::~EventSystem() {
  instance = nullptr;
}

uint32_t EventSystem::dispatchQueue() {
  uint32_t count = 0;
  while (this->eventQueue.size()) {
    auto event = std::move(this->eventQueue.front());
    this->eventQueue.pop();
    count += this->emit(*event);
  }
  return count;
}