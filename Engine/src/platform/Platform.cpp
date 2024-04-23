#include "engine/platform/Platform.h"
#include <engine/utils/logger.h>
#include <engine/platform/input/InputManager.h>

using Engine::Platform;

Platform::Platform(const WindowSpecs specs) {
  this->setup(specs);
}

Platform::~Platform() {
  delete this->input;
  delete this->window;
}

Platform::Platform(glm::uvec2 size, const std::string_view title) {
  this->setup(WindowSpecs{ size, title });
}

Platform::Platform(uint32_t width, uint32_t height, const std::string_view title) {
  this->setup(WindowSpecs{ glm::uvec2{width, height}, title });
}

void Platform::setup(const WindowSpecs spec) {
  this->window = new Window(*this, spec);
  this->input = new Input::InputManager(*this);
}

bool Platform::init() {
  try {
    this->window->init();
    this->input->init();
  } catch (const std::exception& e) {
    LOG_ERROR("Failed to initialize platform: {}", e.what());
    return false;
  }
  return true;
}

void Platform::update() {
  this->input->update();
  this->window->pollEvents();
}