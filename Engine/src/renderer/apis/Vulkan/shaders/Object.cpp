#include "renderer/apis/Vulkan/shaders/Object.h"
#include "renderer/apis/Vulkan/VulkanRenderer.h"

using namespace Engine::Renderers::Vulkan::Shaders;

Object::Object(Renderer& ctx, RenderPass& renderPass)
  : Base(ctx, Object::StagesName), renderPass(renderPass), ubo(ctx.getDevice(), ctx.getSwapchain().getMaxFramesInFlight()) {
  this->init();
}

Object::~Object() {}

void Object::init() {
  auto vertexStage = this->addStage<BuiltinStage>(StageType::Vertex);
  auto fragStage = this->addStage<BuiltinStage>(StageType::Fragment);
  Pipeline::ConfigInfo configInfo = {};
  Pipeline::SetupDefaultConfigInfo(configInfo);
  // configInfo.enableRasterizationCulling();
  configInfo.bindingDescriptions = Vertex::GetBindingDescriptions();
  configInfo.attributeDescriptions = Vertex::GetAttributeDescriptions();
  configInfo.renderPass = this->renderPass;
  configInfo.stages = {
    vertexStage->getPipelineShaderStageCreateInfo(),
    fragStage->getPipelineShaderStageCreateInfo()
  };
  // Descriptors
  uint32_t maxFramesInFlight = this->ctx.getSwapchain().getMaxFramesInFlight();
  this->globalDescriptorPool = std::move(DescriptorPool::Builder(this->ctx.getDevice())
    .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxFramesInFlight)
    .setMaxSets(maxFramesInFlight)
    .build());
  this->globalDescriptorSetLayout = std::move(DescriptorSetLayout::Builder(this->ctx.getDevice())
    .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
    .build());
  this->globalDescriptorSets.resize(maxFramesInFlight);
  for (uint32_t i = 0; i < maxFramesInFlight; i++) {
    this->ubo.bind(
      *this->globalDescriptorPool,
      *this->globalDescriptorSetLayout,
      i, 0,
      this->globalDescriptorSets[i]
    );
  }
  std::vector<VkDescriptorSetLayout> setLayouts = { *this->globalDescriptorSetLayout };
  configInfo.descriptorSetLayouts = setLayouts;

  // push constants
  configInfo.pushConstantRanges = {
    { VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4) }
  };

  this->Base::init(configInfo);
}

std::vector<VkVertexInputBindingDescription> Object::Vertex::GetBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Object::Vertex::GetAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

  attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
  attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
  // attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
  // attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

  return attributeDescriptions;
}

void Object::use(VkFrameInfo& frameInfo) {
  this->pipeline->bind(frameInfo.cmdBuffer);
  vkCmdBindDescriptorSets(
    frameInfo.cmdBuffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    this->pipeline->getLayout(),
    0, 1, &frameInfo.globalDescriptorSet,
    0, nullptr
  );
}

void Object::updateGlobalUniforms(VkFrameInfo& frameInfo) {
  GlobalUbo& uboData = this->ubo;
  uboData = frameInfo.shared.globalUbo;
  this->ubo.update(frameInfo.frameIndex);
}
