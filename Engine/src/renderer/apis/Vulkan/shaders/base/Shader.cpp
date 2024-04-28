#include "renderer/apis/Vulkan/shaders/defines.h"

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

Base::Base(Device& device, const Pipeline::ConfigInfo& pipelineConfigInfo)
  : device(device)/* , pipeline{ device, *this, pipelineConfigInfo } */ {}