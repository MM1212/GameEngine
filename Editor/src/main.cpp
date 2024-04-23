#include <engine/core/Entrypoint.h>
#include "EditorLayer.h"

namespace Editor {
  class App : public Engine::Application {
  public:
    App(
      const Engine::ApplicationInfo& info
    ) : Engine::Application(info) {
      this->pushLayer<MainLayer>();
    }
  };
}

namespace Engine {
  Application* CreateApplication(ApplicationCmdArgs args) {
    ApplicationInfo info{};

    info.windowInfo.size = { 1280, 720 };
    info.windowInfo.title = "Editor";
    info.windowInfo.resizable = true;
    info.args = args;
    return new Editor::App(info);
  }
}