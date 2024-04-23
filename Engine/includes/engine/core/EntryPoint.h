#include "Application.h"
#include <engine/utils/logger.h>

extern Engine::Application* Engine::CreateApplication(Engine::ApplicationCmdArgs args);

int main(int argc, char** argv) {
  Engine::Logger::Init();
  Engine::ApplicationCmdArgs args{ static_cast<uint32_t>(argc), argv };

  auto app = Engine::CreateApplication(args);
  if (!app->init()) {
    return -1;
  }
  app->run();
  delete app;
  return 0;
}