#include "renderer/apis/Vulkan/shaders/Object.h"

using namespace Engine::Renderers::Vulkan::Shaders;

static Pipeline::ConfigInfo createPipelineConfigInfo() {
  Pipeline::ConfigInfo configInfo = {};
  Pipeline::SetupDefaultConfigInfo(configInfo);
  return configInfo;
}

Object::Object(Device& device) : Base(device, createPipelineConfigInfo()) {
  this->init();
}

Object::~Object() {}

void Object::init() {
  this->addStage<BuiltinStage>(StageType::Vertex, "builtin.object");
  this->addStage<BuiltinStage>(StageType::Fragment, "builtin.object");
}
