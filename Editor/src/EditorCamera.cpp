#include <EditorCamera.h>
#include <engine/platform/Window.h>
#include <engine/events/EventSystem.h>
#include <engine/input/Input.h>
#include <engine/core/Coordinates.h>

#include <engine/utils/logger.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <limits>

using namespace Engine;
using Editor::EditorCamera;

EditorCamera::EditorCamera() {
  this->transform.translation.z = 5.f;
  this->transform.rotation.y = glm::pi<float>();
  this->setPerspective(45.f, .1f, 1000.f);
  EventSystem::Get()->on<WindowResizeEvent>([this](const WindowResizeEvent& e) {
    this->setViewportSize(e.width, e.height);
  });
}

void EditorCamera::onUpdate(DeltaTime dt) {
  float moveSpeed = this->moveSpeed;
  float lookSpeed = this->lookSpeed;

  bool changed = false;
  glm::vec3 rotation{ 0.f };

  // Camera Arrows Rotation
  if (Input::IsKeyPressed(Input::Key::Right))
    rotation.y += 1.f;
  if (Input::IsKeyPressed(Input::Key::Left))
    rotation.y -= 1.f;
  if (Input::IsKeyPressed(Input::Key::Up))
    rotation.x -= 1.f;
  if (Input::IsKeyPressed(Input::Key::Down))
    rotation.x += 1.f;

  if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
    this->transform.rotation += glm::normalize(rotation) * lookSpeed * static_cast<float>(dt);
    this->transform.rotation.x = glm::clamp(this->transform.rotation.x, -glm::quarter_pi<float>(), glm::quarter_pi<float>());
    this->transform.rotation.y = glm::mod(this->transform.rotation.y, glm::two_pi<float>());
    changed = true;
  }

  const glm::vec3 forward = glm::normalize(glm::vec3(this->inverseViewMatrix[2]));
  const glm::vec3 right = glm::normalize(glm::cross(forward, Coordinates::Up<glm::vec3>));

  glm::vec3 movement{ 0.f };

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
  frameInfo.uploadCameraParameters(this->viewMatrix, this->projection, this->projectionViewMatrix, this->inverseViewMatrix);
}

void EditorCamera::onProjectionUpdate() {
  this->viewMatrix = glm::eulerAngleXYZ(this->transform.rotation.x, this->transform.rotation.y, this->transform.rotation.z);
  this->viewMatrix = glm::translate(this->viewMatrix, this->transform.translation);
  this->projectionViewMatrix = this->projection * this->viewMatrix;
  this->inverseViewMatrix = glm::inverse(this->viewMatrix);
  // LOG_APP_INFO("Camera:\n - Position: {}\n - Rotation: {}\n - View Matrix:\n{}\n - Projection View Matrix:\n{}\n - Inverse View Matrix:\n{}",
  //   glm::to_string(this->transform.translation),
  //   glm::to_string(glm::degrees(this->transform.rotation)),
  //   glm::to_string(this->viewMatrix),
  //   glm::to_string(this->projectionViewMatrix),
  //   glm::to_string(this->inverseViewMatrix)
  // );
}