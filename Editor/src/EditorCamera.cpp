#include <EditorCamera.h>
#include <engine/platform/Window.h>
#include <engine/events/EventSystem.h>
#include <engine/input/Input.h>
#include <engine/core/Coordinates.h>

#include <engine/utils/logger.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <limits>

using namespace Engine;
using Editor::EditorCamera;

EditorCamera::EditorCamera() {
  this->transform.translation.z = 5.f;
  this->transform.rotation.y = 0.f;
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
    rotation.y -= 1.f;
  if (Input::IsKeyPressed(Input::Key::Left))
    rotation.y += 1.f;
  if (Input::IsKeyPressed(Input::Key::Up))
    rotation.x += 1.f;
  if (Input::IsKeyPressed(Input::Key::Down))
    rotation.x -= 1.f;

  if (glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()) {
    this->transform.rotation += glm::normalize(rotation) * lookSpeed * static_cast<float>(dt);
    this->transform.rotation.x = glm::clamp(this->transform.rotation.x, -glm::quarter_pi<float>(), glm::quarter_pi<float>());
    this->transform.rotation.y = glm::mod(this->transform.rotation.y, glm::two_pi<float>());
    changed = true;
  }

  const glm::vec3 forward = glm::normalize(glm::vec3(this->inverseViewMatrix[2])) * -1.f;
  const glm::vec3 right = glm::normalize(glm::vec3(this->inverseViewMatrix[0]));

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
  frameInfo.globalUbo.projection = this->projection;
  frameInfo.globalUbo.view = this->viewMatrix;
  frameInfo.globalUbo.projectionView = this->projectionViewMatrix;
  frameInfo.globalUbo.inverseView = this->inverseViewMatrix;
}

void EditorCamera::onProjectionUpdate() {
  const float c3 = glm::cos(this->transform.rotation.z);
  const float s3 = glm::sin(this->transform.rotation.z);
  const float c2 = glm::cos(this->transform.rotation.x);
  const float s2 = glm::sin(this->transform.rotation.x);
  const float c1 = glm::cos(this->transform.rotation.y);
  const float s1 = glm::sin(this->transform.rotation.y);
  const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
  const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
  const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
  this->viewMatrix = glm::mat4{ 1.f };
  this->viewMatrix[0][0] = u.x;
  this->viewMatrix[1][0] = u.y;
  this->viewMatrix[2][0] = u.z;
  this->viewMatrix[0][1] = v.x;
  this->viewMatrix[1][1] = v.y;
  this->viewMatrix[2][1] = v.z;
  this->viewMatrix[0][2] = w.x;
  this->viewMatrix[1][2] = w.y;
  this->viewMatrix[2][2] = w.z;
  this->viewMatrix[3][0] = -glm::dot(u, this->transform.position);
  this->viewMatrix[3][1] = -glm::dot(v, this->transform.position);
  this->viewMatrix[3][2] = -glm::dot(w, this->transform.position);
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