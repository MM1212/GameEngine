#include "renderer/apis/Vulkan/shaders/defines.h"

#include <renderer/logger.h>

#include <format>

namespace Engine::Renderers::Vulkan::Shaders {

  Stage::Stage(
    Device& device,
    Base& shader,
    const std::vector<uint8_t>& code,
    StageType type
  ) : device(device), shader(shader), type(type) {
    this->init(code);
  }

  Stage::~Stage() {
    if (this->handle != VK_NULL_HANDLE)
      vkDestroyShaderModule(this->device, this->handle, this->device.getAllocator());
  }

  VkPipelineShaderStageCreateInfo Stage::getPipelineShaderStageCreateInfo() const {
    VkPipelineShaderStageCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    createInfo.stage = GetVkFlags(this->type);
    createInfo.module = this->handle;
    createInfo.pName = "main";
    createInfo.flags = 0;
    createInfo.pNext = nullptr;
    createInfo.pSpecializationInfo = nullptr;
    return createInfo;
  }

  std::string_view Stage::GetExtension(StageType type) {
    switch (type) {
      case StageType::Vertex: return "vert";
      case StageType::Fragment: return "frag";
    }
    return "";
  }

  VkShaderStageFlagBits Stage::GetVkFlags(StageType type) {
    switch (type) {
      case StageType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
      case StageType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  }

  void Stage::init(const std::vector<uint8_t>& code) {
    VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VK_CHECK(vkCreateShaderModule(this->device, &createInfo, this->device.getAllocator(), &this->handle));
  }

  std::string FileStage::ConstructFilePath(StageType type, const std::string& path) {
    return std::format("{}.{}.spv", path, GetExtension(type));
  }

  FileStage::FileStage(
    Device& device,
    Base& shader,
    StageType type,
    const std::string& filePath
  ) : Stage(device, shader, Base::ReadFile(ConstructFilePath(type, filePath)), type), filePath(filePath) {
    LOG_RENDERER_INFO("Shader stage {} loaded from {}", GetExtension(type), filePath);
  }

  std::string BuiltinStage::ConstructId(StageType type, std::string_view id) {
    std::stringstream ss;
    ss << shadersPath << id;
    return ss.str();
  }

  BuiltinStage::BuiltinStage(
    Device& device,
    Base& shader,
    StageType type,
    std::string_view id
  ) : FileStage(device, shader, type, ConstructId(type, id)) {}

  BuiltinStage::BuiltinStage(
    Device& device,
    Base& shader,
    StageType type
  ) : FileStage(device, shader, type, ConstructId(type, shader.getName())) {}
}
