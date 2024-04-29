#include "renderer/apis/Vulkan/shaders/Object.h"

using namespace Engine::Renderers::Vulkan::Shaders;

Object::Object(Device& device, RenderPass& renderPass) : Base(device, Object::StagesName), renderPass(renderPass) {
  this->init();
}

Object::~Object() {}

void Object::init() {
  auto vertexStage = this->addStage<BuiltinStage>(StageType::Vertex);
  auto fragStage = this->addStage<BuiltinStage>(StageType::Fragment);
  Pipeline::ConfigInfo configInfo = {};
  Pipeline::SetupDefaultConfigInfo(configInfo);
  configInfo.enableRasterizationCulling();
  configInfo.bindingDescriptions = Vertex::GetBindingDescriptions();
  configInfo.attributeDescriptions = Vertex::GetAttributeDescriptions();
  configInfo.renderPass = this->renderPass;
  configInfo.stages = {
    vertexStage->getPipelineShaderStageCreateInfo(),
    fragStage->getPipelineShaderStageCreateInfo()
  };
  // TODO: add descriptor set layout
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
  // attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
  // attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
  // attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

  return attributeDescriptions;
}

void Object::use(CommandBuffer& cmdBuffer) {
  this->pipeline->bind(cmdBuffer);
}