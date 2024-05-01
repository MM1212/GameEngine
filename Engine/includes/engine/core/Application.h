#pragma once

#include <engine/platform/Platform.h>
#include <engine/utils/logger.h>
#include <engine/utils/asserts.h>
#include <engine/utils/memory.h>
#include <engine/events/EventSystem.h>
#include <engine/renderer/RendererAPI.h>
#include <engine/scene/Scene.h>
#include "LayersManager.h"
#include "DeltaTime.h"

namespace Engine {
  struct ApplicationCmdArgs {
    uint32_t count = 1;
    char** args = nullptr;
    std::string_view operator[](uint32_t index) const {
      APP_ASSERT(index < count, "Index out of bounds");
      return args[index];
    }
  };
  struct ApplicationVersion {
    uint32_t major = 1;
    uint32_t minor = 0;
    uint32_t patch = 0;
  };
  struct ApplicationInfo {
    WindowSpecs windowInfo;
    std::string_view workingDirectory;
    ApplicationCmdArgs args;
    ApplicationVersion version{};
  };
  class Application {
  public:
    Application(const ApplicationInfo& info);
    virtual ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    static Application* Get() { return s_instance; }
    virtual bool init();
    void run();

    void pushLayer(Ref<AppLayer> layer) { this->layersManager.pushLayer(layer); }
    template <typename T, typename... Args>
    Ref<T> pushLayer(Args&&... args) {
      return this->layersManager.pushLayer<T>(std::forward<Args>(args)...);
    }
    LayersManager& getLayersManager() { return this->layersManager; }

    Window& getWindow() { return *this->platform.window; }
    Input::InputManager& getInputManager() { return *this->platform.input; }
    bool isRunning() const { return this->running; }
    bool isSuspended() const { return this->suspended; }
  protected:
    ApplicationInfo spec;
    EventSystem eventSystem;
    Platform platform;
    std::unique_ptr<Renderer> renderer;
    bool running = false;
    bool suspended = false;

  private:
    LayersManager layersManager;

    void onUpdate(DeltaTime dt);
    void onRender(FrameInfo& frameInfo);
    void onBeginFrame(FrameInfo& frameInfo);
    void onEndFrame(FrameInfo& frameInfo);
  private:
    static Application* s_instance;
  };

  Application* CreateApplication(ApplicationCmdArgs args);
}