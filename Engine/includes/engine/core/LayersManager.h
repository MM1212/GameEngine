#pragma once

#include <engine/utils/memory.h>
#include <vector>
#include "AppLayer.h"
#include <engine/core/Callbacks.h>

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

    Callback<DeltaTime>& getOnUpdateCallback() { return this->onUpdateCallback; }
    Callback<FrameInfo&>& getOnRenderCallback() { return this->onRenderCallback; }
    Callback<FrameInfo&>& getOnBeginFrameCallback() { return this->onBeginFrameCallback; }
    Callback<FrameInfo&>& getOnEndFrameCallback() { return this->onEndFrameCallback; }
  private:
    void onUpdate(DeltaTime dt);
    void onRender(FrameInfo& frameInfo);
    void onBeginFrame(FrameInfo& frameInfo);
    void onEndFrame(FrameInfo& frameInfo);
    friend class Application;
    friend class AppLayer;
  private:
    std::vector<Ref<AppLayer>> layers;
    std::vector<Ref<AppLayer>> overlays;

    Callback<DeltaTime> onUpdateCallback;
    Callback<FrameInfo&> onRenderCallback;
    Callback<FrameInfo&> onBeginFrameCallback;
    Callback<FrameInfo&> onEndFrameCallback;
  };
}