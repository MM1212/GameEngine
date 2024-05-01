#pragma once

#include <glm/glm.hpp>
#include <engine/renderer/Camera.h>

namespace Engine::Components {
  struct Camera {
    Engine::Camera sceneCamera;
    bool primary = true;

    Camera() = default;
    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
  };
}