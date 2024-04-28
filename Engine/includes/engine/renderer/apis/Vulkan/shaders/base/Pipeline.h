#pragma once

#include "renderer/apis/Vulkan/defines.h"
#include "renderer/apis/Vulkan/Device.h"
#include "renderer/apis/Vulkan/CommandBuffer.h"

#include <vector>

namespace Engine::Renderers::Vulkan::Shaders {
  class Base;
  struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    PipelineConfigInfo(PipelineConfigInfo&& other) = default;
    PipelineConfigInfo& operator=(PipelineConfigInfo&& other) = default;

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizerInfo;
    VkPipelineMultisampleStateCreateInfo multisamplingInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendingInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
  };
  class Pipeline {
  public:
    using ConfigInfo = PipelineConfigInfo;
    Pipeline(
      Device& device,
      Base& shader,
      const ConfigInfo& configInfo
    );
    ~Pipeline();
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(CommandBuffer& cmdBuffer);
    static void SetupDefaultConfigInfo(ConfigInfo& configInfo);
    static void EnableAlphaBlending(ConfigInfo& configInfo);
  private:
    void createGraphicsPipeline(
      const ConfigInfo& configInfo
    );

    Device& device;
    Base& shader;
    VkPipeline handle = VK_NULL_HANDLE;
  };
};