#pragma once

#include "defines.h"
#include "Device.h"
#include "CommandBuffer.h"

#include <vector>

namespace Engine::Renderers::Vulkan::Shaders {
  class Base;
  struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    PipelineConfigInfo(PipelineConfigInfo&& other) = default;
    PipelineConfigInfo& operator=(PipelineConfigInfo&& other) = default;

    std::vector<VkPipelineShaderStageCreateInfo> stages;
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
    uint32_t subpass = 0;

    // create pipeline layout data
    VkRenderPass renderPass = nullptr;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    std::vector<VkPushConstantRange> pushConstantRanges;

    PipelineConfigInfo& enableAlphaBlending();
    PipelineConfigInfo& enableWireframe();
    PipelineConfigInfo& enableRasterizationCulling(VkCullModeFlagBits mode = VK_CULL_MODE_BACK_BIT, VkFrontFace face = VK_FRONT_FACE_COUNTER_CLOCKWISE);
  };
  class Pipeline {
  public:
    using ConfigInfo = PipelineConfigInfo;
    Pipeline(
      Device& device,
      const ConfigInfo& configInfo
    );
    ~Pipeline();
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void bind(CommandBuffer& cmdBuffer, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);
    static void SetupDefaultConfigInfo(ConfigInfo& configInfo);
  private:
    void init(ConfigInfo& configInfo);
    void createLayout(
      const ConfigInfo& configInfo
    );
    void createGraphicsPipeline(
      const ConfigInfo& configInfo
    );

    Device& device;
    VkPipeline handle = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;
  };
};