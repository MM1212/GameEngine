#pragma once

#include "ID.h"
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "RigidBody2D.h"
#include "Billboard.h"
#include "Lights.h"

namespace Engine {
  using IDComponent = Components::ID;
  using TransformComponent = Components::Transform;
  // using MeshComponent = Components::Mesh;
  using CameraComponent = Components::Camera;
  using RigidBody2DComponent = Components::RigidBody2D;
  using BillboardComponent = Components::Billboard;
  using PointLightComponent = Components::PointLight;
  using GlobalLightComponent = Components::GlobalLight;
}