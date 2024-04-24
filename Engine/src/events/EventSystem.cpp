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