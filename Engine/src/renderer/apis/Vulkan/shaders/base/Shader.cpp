#include "renderer/apis/Vulkan/shaders/defines.h"
#include "renderer/apis/Vulkan/VulkanRenderer.h"

#include <renderer/logger.h>

using namespace Engine::Renderers::Vulkan::Shaders;

std::vector<uint8_t> Base::ReadFile(const std::string_view filePath) {
  std::ifstream file(filePath.data(), std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file: " + std::string(filePath));
  }

  std::size_t fileSize = static_cast<std::size_t>(file.tellg());
  std::vector<uint8_t> buffer(fileSize);

  file.seekg(0);
  file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
  file.close();
  return buffer;
}

Base::Base(Renderer& ctx, const std::string& name)
  : ctx(ctx), name(name) {}

Base::Base(Renderer& ctx, std::string_view name)
  : ctx(ctx), name(name) {}

Base::~Base() {
  LOG_RENDERER_INFO("Destroyed shader {}", this->name);
}

// needs to be called after all stages are added
void Base::init(const PipelineConfigInfo& pipelineConfigInfo) {
  this->pipeline = MakeScope<Pipeline>(this->ctx.getDevice(), pipelineConfigInfo);
  LOG_RENDERER_INFO("Created shader {}", this->name);
}