#include "EditorLayer.h"
#include <engine/core/Application.h>

using Editor::MainLayer;

MainLayer::MainLayer() : Engine::AppLayer("EditorLayer") {}

void MainLayer::onAttach() {
  auto& window = this->app.getWindow();
  this->camera.setViewportSize(window.getWidth(), window.getHeight());
  this->cbHandles.onUpdate = this->manager().getOnUpdateCallback().connect([this](Engine::DeltaTime dt) {
    this->onUpdate(dt);
    return true;
  });
  this->cbHandles.onBeginFrame = this->manager().getOnBeginFrameCallback().connect([this](Engine::FrameInfo& frameInfo) {
    this->onBeginFrame(frameInfo);
    return true;
  });
}
void MainLayer::onDetach() {
  this->manager().getOnUpdateCallback().disconnect(this->cbHandles.onUpdate);
  this->manager().getOnBeginFrameCallback().disconnect(this->cbHandles.onBeginFrame);
}
void MainLayer::onUpdate(Engine::DeltaTime dt) {
  this->camera.onUpdate(dt);
}
void MainLayer::onRender(Engine::FrameInfo& frameInfo) {}
void MainLayer::onBeginFrame(Engine::FrameInfo& frameInfo) {
  this->camera.onRender(frameInfo);
}
void MainLayer::onEndFrame(Engine::FrameInfo& frameInfo) {}