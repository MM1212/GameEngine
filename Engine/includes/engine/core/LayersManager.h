#pragma once

#include <engine/utils/memory.h>
#include <vector>
#include "AppLayer.h"

namespace Engine {
  class LayersManager {
  public:
    LayersManager() = default;
    ~LayersManager();

    bool pushLayer(Ref<AppLayer> layer);
    bool pushOverlay(Ref<AppLayer> overlay);
    bool popLayer(Ref<AppLayer> layer);
    bool popOverlay(Ref<AppLayer> overlay);

    template<typename T, typename... Args>
    Ref<T> pushLayer(Args&&... args) {
      auto layer = MakeRef<T>(std::forward<Args>(args)...);
      this->pushLayer(layer);
      return layer;
    }
    template<typename T, typename... Args>
    Ref<T> pushOverlay(Args&&... args) {
      auto overlay = MakeRef<T>(std::forward<Args>(args)...);
      this->pushOverlay(overlay);
      return overlay;
    }
  private:
    void onUpdate(DeltaTime dt);
    void onRender(DeltaTime dt);
    friend class Application;
  private:
    std::vector<Ref<AppLayer>> layers;
    std::vector<Ref<AppLayer>> overlays;
  };
}