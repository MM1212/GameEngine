#include "engine/core/Application.h"
#include <engine/utils/logger.h>
#include <engine/platform/input/InputManager.h>

#include <filesystem>

namespace Engine {
  Application* Application::s_instance = nullptr;

  Application::Application(const ApplicationInfo& info)
    : spec(info), platform(info.windowInfo) {
    APP_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    if (!spec.workingDirectory.empty()) {
      std::filesystem::current_path(spec.workingDirectory);
    }
  }

  bool Application::init() {
    if (!this->platform.init()) {
      LOG_CRITICAL("Failed to initialize platform!");
      return false;
    }
    return true;
  }

  void Application::onUpdate(DeltaTime dt) {
    this->layersManager.onUpdate(dt);
  }
  void Application::onRender(DeltaTime dt) {
    this->layersManager.onRender(dt);
  }

  void Application::run() {
    this->running = true;
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (this->running) {
      this->platform.update();
      auto now = std::chrono::high_resolution_clock::now();
      DeltaTime dt = std::chrono::duration<float, std::chrono::seconds::period>(now - lastTime).count();
      lastTime = now;
      // if (this->platform.window.wasResized()) {
      //   this->onWindowResize();
      //   this->platform.window.resetResizedFlag();
      // }
      if (this->platform.shouldExit()) {
        this->running = false;
        continue;
      }

      this->onUpdate(dt);
      this->onRender(dt);
    }
  }
}