#include "renderer/apis/Vulkan/shaders/defines.h"

#include <utils/asserts.h>

#include <fstream>
#include <stdexcept>
#include <cassert>

using namespace Engine::Renderers::Vulkan::Shaders;

Pipeline::Pipeline(
  Device& device,
  Base& shader,
  const ConfigInfo& configInfo
) : device{ device }, shader{ shader } {
  this->createGraphicsPipeline(configInfo);
}

Pipeline::~Pipeline() {
  vkDestroyPipeline(this->device.getHandle(), this->handle, nullptr);
}

void Pipeline::createGraphicsPipeline(
  const ConfigInfo& configInfo
) {
  ASSERT(configInfo.pipelineLayout != VK_NULL_HANDLE, "pipeline layout is null");
  ASSERT(configInfo.renderPass != VK_NULL_HANDLE, "render pass is null");

  auto vertShaderStage = this->shader.getStage(StageType::Vertex);
  auto fragShaderStage = this->shader.getStage(StageType::Fragment);
  ASSERT(vertShaderStage != nullptr, "vertex shader is null");
  ASSERT(fragShaderStage != nullptr, "fragment shader is null");

  VkPipelineShaderStageCreateInfo shaderStages[2] = {
    vertShaderStage->getPipelineShaderStageCreateInfo(),
    fragShaderStage->getPipelineShaderStageCreateInfo()
  };

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(configInfo.bindingDescriptions.size());
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(configInfo.attributeDescriptions.size());
  vertexInputInfo.pVertexBindingDescriptions = configInfo.bindingDescriptions.data();
  vertexInputInfo.pVertexAttributeDescriptions = configInfo.attributeDescriptions.data();

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
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

  if (vkCreateGraphicsPipelines(
    this->device.getHandle(),
    VK_NULL_HANDLE, 1,
    &pipelineInfo, nullptr,
    &this->handle
  ) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline");
  }
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
  configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
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

  configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
  configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamicStateEnables.size());
  configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
  configInfo.dynamicStateInfo.flags = 0;

  // TODO: Mesh vertex data
  // configInfo.bindingDescriptions = Model::Vertex::GetBindingDescriptions();
  // configInfo.attributeDescriptions = Model::Vertex::GetAttributeDescriptions();
}

void Pipeline::EnableAlphaBlending(Pipeline::ConfigInfo& configInfo) {
  configInfo.colorBlendAttachment.colorWriteMask =
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
    VK_COLOR_COMPONENT_A_BIT;
  configInfo.colorBlendAttachment.blendEnable = VK_TRUE;
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
}

void Pipeline::bind(CommandBuffer& cmdBuffer) {
  vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->handle);
}