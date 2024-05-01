#pragma once

#include <string_view>
#include <engine/core/DeltaTime.h>
#include <memory>
#include <engine/renderer/FrameInfo.h>
#include <engine/core/Callbacks.h>

namespace Engine {
  class Application;
  class LayersManager;
  class AppLayer : public std::enable_shared_from_this<AppLayer> {
  public:
    AppLayer(const std::string_view name = "AppLayer");
    virtual ~AppLayer() = default;

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(DeltaTime dt) {}
    virtual void onRender(FrameInfo& frameInfo) {}
    virtual void onBeginFrame(FrameInfo& frameInfo) {}
    virtual void onEndFrame(FrameInfo& frameInfo) {}

    const std::string_view getName() const { return this->debugName; }
  protected:
    LayersManager& manager() const;

  protected:
    std::string_view debugName;
    Application& app;
  };
}