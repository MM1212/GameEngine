#include <EditorCamera.h>
#include <engine/platform/Window.h>
#include <engine/events/EventSystem.h>
#include <engine/input/Input.h>
#include <engine/core/Coordinates.h>

#include <limits>

using namespace Engine;
using Editor::EditorCamera;

EditorCamera::EditorCamera() {
  this->setPerspective(45.f, .1f, 1000.f);
  this->transform.translation.z = 5.f;
  EventSystem::Get()->on<WindowResizeEvent>([this](const WindowResizeEvent& e) {
    this->setViewportSize(e.width, e.height);
  });
}

void EditorCamera::onUpdate(DeltaTime dt) {
  float moveSpeed = this->moveSpeed;
  float lookSpeed = this->lookSpeed;

  bool changed = false;
  glm::vec3 rotation{0.f};

  // Camera Arrows Rotation
  if (Input::IsKeyPressed(Input::Key::Right))
    rotation.y -= 1.f;
  if (Input::IsKeyPressed(Input::Key::Left))
    rotation.y += 1.f;
  if (Input::IsKeyPressed(Input::Key::Up))
    rotation.x += 1.f;
  if (Input::IsKeyPressed(Input::Key::Down))
    rotation.x -= 1.f;

  if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
    this->transform.rotation += glm::normalize(rotation) * lookSpeed * static_cast<float>(dt);
    changed = true;
  }

  const glm::vec3 forward = this->transform.forward();
  const glm::vec3 right = this->transform.right();

  glm::vec3 movement{0.f};

  // Camera WASD Movement
  if (Input::IsKeyPressed(Input::Key::W))
    movement += forward;
  if (Input::IsKeyPressed(Input::Key::S))
    movement -= forward;
  if (Input::IsKeyPressed(Input::Key::D))
    movement += right;
  if (Input::IsKeyPressed(Input::Key::A))
    movement -= right;
  
  if (Input::IsKeyPressed(Input::Key::Q))
    movement += Coordinates::Up<glm::vec3>;
  if (Input::IsKeyPressed(Input::Key::E))
    movement -= Coordinates::Up<glm::vec3>;
  
  if (Input::IsKeyPressed(Input::Key::Space))
    movement += Coordinates::Up<glm::vec3>;
  
  if (Input::IsKeyPressed(Input::Key::LeftShift))
    moveSpeed *= 2.f;
  else if (Input::IsKeyPressed(Input::Key::LeftControl))
    moveSpeed *= .5f;
  
  if (glm::dot(movement, movement) > std::numeric_limits<float>::epsilon()) {
    this->transform.translation += glm::normalize(movement) * moveSpeed * static_cast<float>(dt);
    changed = true;
  }

  if (changed)
    this->onProjectionUpdate();
}

void EditorCamera::onRender(FrameInfo& frameInfo) {
  frameInfo.globalUbo.projection = this->projection;
  frameInfo.globalUbo.view = this->viewMatrix;
  frameInfo.globalUbo.viewProjection = this->projectionViewMatrix;
  frameInfo.globalUbo.inverseView = this->inverseViewMatrix;
}

void EditorCamera::onProjectionUpdate() {
  this->viewMatrix = glm::inverse(static_cast<glm::mat4>(this->transform));
  this->projectionViewMatrix = this->projection * this->viewMatrix;
  this->inverseViewMatrix = glm::inverse(this->viewMatrix);
}