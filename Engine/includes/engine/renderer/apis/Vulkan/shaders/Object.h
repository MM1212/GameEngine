#pragma once

#include "defines.h"
#include "renderer/apis/Vulkan/RenderPass.h"
#include "renderer/apis/Vulkan/UniformBuffer.h"

#include <glm/glm.hpp>
#include <string_view>


namespace Engine::Renderers::Vulkan {
  class Renderer;
  namespace Shaders {
    class Object : public Base {
    public:
      struct Vertex {
        glm::vec3 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
      };
      static constexpr std::string_view StagesName = "builtin.object";
      Object(Renderer& ctx, RenderPass& renderPass);
      ~Object();

      Object(const Object&) = delete;
      Object& operator=(const Object&) = delete;

      void use(VkFrameInfo& frameInfo) override;

      VkDescriptorSet getGlobalDescriptorSet(uint32_t frameIndex) const {
        return this->globalDescriptorSets[frameIndex];
      }

      void updateGlobalUniforms(VkFrameInfo& frameInfo);
    private:
      void init();
    private:
      RenderPass& renderPass;
      Ref<DescriptorPool> globalDescriptorPool;
      Ref<DescriptorSetLayout> globalDescriptorSetLayout;
      std::vector<VkDescriptorSet> globalDescriptorSets;
      UniformBuffer<GlobalUbo> ubo;
    };
  }
};