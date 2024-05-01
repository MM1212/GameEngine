#pragma once

#include <glm/glm.hpp>

namespace Engine::Components {
  struct RigidBody2D {
    float mass{1.0f};
    glm::vec2 velocity{.0f, .0f};

    RigidBody2D() = default;
    RigidBody2D(const RigidBody2D&) = default;
    RigidBody2D& operator=(const RigidBody2D&) = default;
    RigidBody2D(float mass) : mass{mass} {}
    RigidBody2D(float mass, const glm::vec2& velocity) : mass{mass}, velocity{velocity} {}
  };
}