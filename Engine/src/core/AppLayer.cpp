#include "engine/core/AppLayer.h"
#include "engine/core/LayersManager.h"

#include <engine/core/Application.h>

using Engine::AppLayer;

AppLayer::AppLayer(const std::string_view name)
  : debugName(name), app(*Application::Get()) { }
