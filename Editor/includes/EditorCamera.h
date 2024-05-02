#pragma once

#include <engine/renderer/Camera.h>
#include <engine/scene/components/Transform.h>
#include <engine/renderer/FrameInfo.h>
#include <engine/core/DeltaTime.h>

namespace Editor {
  class EditorCamera : public Engine::Camera {
  public:
    EditorCamera();
    ~EditorCamera() = default;

    void onUpdate(Engine::DeltaTime dt);
    void onRender(Engine::FrameInfo& frameInfo);
    void onProjectionUpdate() override;

  private:
    glm::mat4 viewMatrix{ 1.0f };
    glm::mat4 projectionViewMatrix{ 1.0f };
    glm::mat4 inverseViewMatrix{ 1.0f };
    Engine::Components::Transform transform{};
    float lookSpeed{ 3.f };
    float moveSpeed{ 10.5f };
  };
}