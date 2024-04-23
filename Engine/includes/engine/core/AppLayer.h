#pragma once

#include <string_view>
#include <engine/core/DeltaTime.h>
#include <memory>

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
    virtual void onRender(DeltaTime dt) {}

    const std::string_view getName() const { return this->debugName; }
  protected:
    std::string_view debugName;
    Application& app;
  };
}