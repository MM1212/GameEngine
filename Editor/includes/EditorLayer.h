#pragma once

#include <engine/core/AppLayer.h>
#include <engine/core/LayersManager.h>
#include <engine/scene/Scene.h>
#include "EditorCamera.h"

namespace Editor {
  class MainLayer : public Engine::AppLayer {
  public:
    MainLayer();
    ~MainLayer() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate(Engine::DeltaTime dt) override;
    virtual void onRender(Engine::FrameInfo& frameInfo) override;
    virtual void onBeginFrame(Engine::FrameInfo& frameInfo) override;
    virtual void onEndFrame(Engine::FrameInfo& frameInfo) override;
  private:
    EditorCamera camera;
    struct {
      uint64_t onUpdate{ static_cast<uint64_t>(-1) };
      uint64_t onRender{ static_cast<uint64_t>(-1) };
      uint64_t onBeginFrame{ static_cast<uint64_t>(-1) };
      uint64_t onEndFrame{ static_cast<uint64_t>(-1) };
    } cbHandles;
  };
}