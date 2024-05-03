#pragma once

#include <glm/glm.hpp>

namespace Engine {
  struct GlobalUbo {
    glm::mat4 view{1.f};
    glm::mat4 projection{1.f};
    glm::mat4 projectionView{1.f};
    glm::mat4 inverseView{1.f};
  };
  struct FrameInfo {
    float deltaTime{};
    GlobalUbo globalUbo{};

    void uploadCameraParameters(
      const glm::mat4& view,
      const glm::mat4& projection,
      const glm::mat4& projectionView,
      const glm::mat4& inverseView
    ) {
      globalUbo.view = view;
      globalUbo.projection = projection;
      globalUbo.projectionView = projectionView;
      globalUbo.inverseView = inverseView;
    }
  };
}