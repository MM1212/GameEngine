#include "renderer/apis/Vulkan/Pipeline.h"

#include <utils/asserts.h>
#include <renderer/logger.h>

#include <fstream>
#include <stdexcept>
#include <format>

using namespace Engine::Renderers::Vulkan::Shaders;

Pipeline::Pipeline(
  Device& device,
  const ConfigInfo& configInfo
) : device{ device } {
  this->init(const_cast<ConfigInfo&>(configInfo));
}

Pipeline::~Pipeline() {
  vkDestroyPipelineLayout(this->device.getHandle(), this->layout, this->device.getAllocator());
  vkDestroyPipeline(this->device.getHandle(), this->handle, this->device.getAllocator());
}

void Pipeline::createLayout(const ConfigInfo& configInfo) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(configInfo.descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = configInfo.descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(configInfo.pushConstantRanges.size());
  pipelineLayoutInfo.pPushConstantRanges = configInfo.pushConstantRanges.data();

  VK_CHECK(vkCreatePipelineLayout(
    this->device.getHandle(),
    &pipelineLayoutInfo,
    this->device.getAllocator(),
    &this->layout
  ));
}

void Pipeline::createGraphicsPipeline(
  const ConfigInfo& configInfo
) {
  ASSERT(configInfo.pipelineLayout != VK_NULL_HANDLE, "pipeline layout is null");
  ASSERT(configInfo.renderPass != VK_NULL_HANDLE, "render pass is null");

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(configInfo.bindingDescriptions.size());
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(configInfo.attributeDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = configInfo.bindingDescriptions.data();
  vertexInputInfo.pVertexAttributeDescriptions = configInfo.attributeDescriptions.data();

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = static_cast<uint32_t>(configInfo.stages.size());
  pipelineInfo.pStages = configInfo.stages.data();
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
  pipelineInfo.pViewportState = &configInfo.viewportInfo;
  pipelineInfo.pRasterizationState = &configInfo.rasterizerInfo;
  pipelineInfo.pMultisampleState = &configInfo.multisamplingInfo;
  pipelineInfo.pColorBlendState = &configInfo.colorBlendingInfo;
  pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
  pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
  pipelineInfo.layout = configInfo.pipelineLayout;
  pipelineInfo.renderPass = configInfo.renderPass;
  pipelineInfo.subpass = configInfo.subpass;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  VkResult result = vkCreateGraphicsPipelines(
    this->device.getHandle(),
    VK_NULL_HANDLE,
    1,
    &pipelineInfo,
    this->device.getAllocator(),
    &this->handle
  );
  if (IsCallResultSuccess(result)) {
    LOG_RENDERER_INFO("Created graphics pipeline");
    return;
  }
  throw std::runtime_error(std::format("Failed to create graphics pipeline - {}", CallResultToString(result, true)));
}

void Pipeline::init(ConfigInfo& configInfo) {
  this->createLayout(configInfo);
  configInfo.pipelineLayout = this->layout;
  this->createGraphicsPipeline(configInfo);
}

void Pipeline::SetupDefaultConfigInfo(Pipeline::ConfigInfo& configInfo) {
  configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  configInfo.viewportInfo.viewportCount = 1;
  configInfo.viewportInfo.pViewports = nullptr;
  configInfo.viewportInfo.scissorCount = 1;
  configInfo.viewportInfo.pScissors = nullptr;

  configInfo.rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  configInfo.rasterizerInfo.depthClampEnable = VK_FALSE;
  configInfo.rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
  configInfo.rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
  configInfo.rasterizerInfo.lineWidth = 1.0f;
  configInfo.rasterizerInfo.cullMode = VK_CULL_MODE_NONE;
  configInfo.rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  configInfo.rasterizerInfo.depthBiasEnable = VK_FALSE;
  configInfo.rasterizerInfo.depthBiasConstantFactor = 0.0f;
  configInfo.rasterizerInfo.depthBiasClamp = 0.0f;
  configInfo.rasterizerInfo.depthBiasSlopeFactor = 0.0f;

  configInfo.multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  configInfo.multisamplingInfo.sampleShadingEnable = VK_FALSE;
  configInfo.multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  configInfo.multisamplingInfo.minSampleShading = 1.0f;
  configInfo.multisamplingInfo.pSampleMask = nullptr;
  configInfo.multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
  configInfo.multisamplingInfo.alphaToOneEnable = VK_FALSE;

  configInfo.colorBlendAttachment.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
  configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  configInfo.colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  configInfo.colorBlendingInfo.logicOpEnable = VK_FALSE;
  configInfo.colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
  configInfo.colorBlendingInfo.attachmentCount = 1;
  configInfo.colorBlendingInfo.pAttachments = &configInfo.colorBlendAttachment;
  configInfo.colorBlendingInfo.blendConstants[0] = 0.0f;
  configInfo.colorBlendingInfo.blendConstants[1] = 0.0f;
  configInfo.colorBlendingInfo.blendConstants[2] = 0.0f;
  configInfo.colorBlendingInfo.blendConstants[3] = 0.0f;

  configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.minDepthBounds = 0.0f;
  configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
  configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.front = {};
  configInfo.depthStencilInfo.back = {};

  configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };
  configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
  configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
  configInfo.dynamicStateInfo.flags = 0;

  // TODO: Mesh vertex data
  // configInfo.bindingDescriptions = Model::Vertex::GetBindingDescriptions();
  // configInfo.attributeDescriptions = Model::Vertex::GetAttributeDescriptions();
}

PipelineConfigInfo& PipelineConfigInfo::enableAlphaBlending() {
  this->colorBlendAttachment.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
  this->colorBlendAttachment.blendEnable = VK_TRUE;
  this->colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  this->colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  this->colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  this->colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  this->colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  this->colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  return *this;
}

PipelineConfigInfo& PipelineConfigInfo::enableWireframe() {
  this->rasterizerInfo.polygonMode = VK_POLYGON_MODE_LINE;
  return *this;
}

PipelineConfigInfo& PipelineConfigInfo::enableRasterizationCulling(VkCullModeFlagBits mode, VkFrontFace face) {
  this->rasterizerInfo.cullMode = mode;
  this->rasterizerInfo.frontFace = face;
  return *this;
}

void Pipeline::bind(CommandBuffer& cmdBuffer, VkPipelineBindPoint bindPoint) {
  vkCmdBindPipeline(cmdBuffer, bindPoint, this->handle);
}