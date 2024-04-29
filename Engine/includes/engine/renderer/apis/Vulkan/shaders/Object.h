#pragma once

#include "defines.h"
#include "renderer/apis/Vulkan/RenderPass.h"

#include <glm/glm.hpp>
#include <string_view>

namespace Engine::Renderers::Vulkan::Shaders {
  class Object : public Base {
  public:
    struct Vertex {
      glm::vec3 position;

      static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };
    static constexpr std::string_view StagesName = "builtin.object";
    Object(Device& device, RenderPass& renderPass);
    ~Object();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;

    void use(CommandBuffer& cmdBuffer) override;
  private:
    void init();
  private:
    RenderPass& renderPass;
  };
};