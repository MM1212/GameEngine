#include "engine/core/Application.h"
#include <engine/utils/logger.h>
#include <engine/input/InputManager.h>

#include <filesystem>

#define BIND_EVENT_FN(fn, EventType) ([fn](Event&ev) {\
  auto& e = dynamic_cast<##EventType&>(ev);\
  fn(e); \
})

namespace Engine {
  Application* Application::s_instance = nullptr;

  Application::Application(const ApplicationInfo& info)
    : spec(info), platform(info.windowInfo) {
    APP_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    if (!spec.workingDirectory.empty()) {
      std::filesystem::current_path(spec.workingDirectory);
    }
    else {
      std::filesystem::path exeFolderPath = std::filesystem::absolute(std::filesystem::path(spec.args[0])).parent_path();
      std::filesystem::path binPath = exeFolderPath.parent_path();
      std::filesystem::current_path(binPath);
    }
    this->eventSystem.on<WindowCloseEvent>([this](WindowCloseEvent& ev) {
      this->running = false;
      return true;
    });
    this->eventSystem.on<WindowResizeEvent>([this](WindowResizeEvent& ev) {
      this->suspended = ev.width == 0 || ev.height == 0;
      if (this->suspended)
        LOG_APP_INFO("Window minimized");
      else
        LOG_APP_INFO("Window resized to {}x{}", ev.width, ev.height);
      return this->suspended;
    }, 101);
  }

  Application::~Application() {
    s_instance = nullptr;
  }

  bool Application::init() {
    if (!this->platform.init()) {
      LOG_CRITICAL("Failed to initialize platform!");
      return false;
    }
    // TODO: Allow the user to choose the renderer API
    this->renderer = RendererAPI::Create(this->spec, this->platform);
    if (!this->renderer) {
      LOG_CRITICAL("Failed to create renderer!");
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
      this->eventSystem.dispatchQueue();
      if (this->suspended) {
        this->platform.window->waitEvents();
        continue;
      }
      auto now = std::chrono::high_resolution_clock::now();
      DeltaTime dt = std::chrono::duration<float, std::chrono::seconds::period>(now - lastTime).count();
      lastTime = now;
      this->onUpdate(dt);
      if (!this->renderer->beginFrame())
        continue;
      this->onRender(dt);
      this->renderer->endFrame();
    }
  }
}