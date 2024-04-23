#include "engine/core/LayersManager.h"
#include <engine/utils/logger.h>

using Engine::LayersManager;

LayersManager::~LayersManager() {
  for (auto& layer : this->layers) {
    layer->onDetach();
  }

  for (auto& overlay : this->overlays) {
    overlay->onDetach();
  }
}

bool LayersManager::pushLayer(Ref<AppLayer> layer) {
  this->layers.emplace(this->layers.begin(), layer);
  layer->onAttach();
  LOG_APP_TRACE("Attached layer {}", layer->getName());
  return true;
}

bool LayersManager::pushOverlay(Ref<AppLayer> overlay) {
  this->overlays.emplace(this->overlays.begin(), overlay);
  overlay->onAttach();
  LOG_APP_TRACE("Attached overlay {}", overlay->getName());
  return true;
}

bool LayersManager::popLayer(Ref<AppLayer> layer) {
  auto it = std::find(this->layers.begin(), this->layers.end(), layer);
  if (it != this->layers.end()) {
    layer->onDetach();
    this->layers.erase(it);
    LOG_APP_INFO("Detached layer {}", layer->getName());
    return true;
  }
  return false;
}

bool LayersManager::popOverlay(Ref<AppLayer> overlay) {
  auto it = std::find(this->overlays.begin(), this->overlays.end(), overlay);
  if (it != this->overlays.end()) {
    overlay->onDetach();
    this->overlays.erase(it);
    LOG_APP_INFO("Detached overlay {}", overlay->getName());
    return true;
  }
  return false;
}

void LayersManager::onUpdate(DeltaTime dt) {
  for (auto& layer : this->layers) {
    layer->onUpdate(dt);
  }
  for (auto& overlay : this->overlays) {
    overlay->onUpdate(dt);
  }
}

void LayersManager::onRender(DeltaTime dt) {
  for (auto& layer : this->layers) {
    layer->onRender(dt);
  }
  for (auto& overlay : this->overlays) {
    overlay->onRender(dt);
  }
}