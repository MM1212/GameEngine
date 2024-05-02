#include "systems/Profiler.hpp"

#include <engine/input/Input.h>

#include <engine/utils/logger.h>

using Editor::Profiler;
using namespace Engine;

void Profiler::update(float deltaTime) {
  if (Input::IsKeyDown(Input::Key::P))
    this->enabled = !this->enabled;
  if (!this->enabled)
    return;
  this->lastOutput -= deltaTime;
  if (this->lastOutput <= 0.f) {
    this->lastOutput = 1.f;
    LOG_APP_INFO("Profiler: {0}ms, fps: {1}", deltaTime * 1000, 1.f / deltaTime);
  }
}