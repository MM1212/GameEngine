#pragma once

#include "renderer/apis/Vulkan/defines.h"
#include "renderer/apis/Vulkan/Device.h"

#include <string_view>
#include <vector>

namespace Engine::Renderers::Vulkan::Shaders {
  class Base;
  enum class StageType {
    Vertex,
    Fragment
  };

  class Stage {
  public:
    Stage() = delete;
    Stage(
      Device& device,
      const std::vector<uint8_t>& code,
      StageType type
    );
    ~Stage();
    Stage(const Stage&) = delete;
    Stage& operator=(const Stage&) = delete;

    Stage(Stage&& other) = default;
    Stage& operator=(Stage&& other) = default;

    operator VkShaderModule() const { return this->handle; }
    VkShaderModule getHandle() const { return this->handle; }
    StageType getType() const { return this->type; }

    VkPipelineShaderStageCreateInfo getPipelineShaderStageCreateInfo() const;

    static std::string_view GetExtension(StageType type);
    static VkShaderStageFlagBits GetVkFlags(StageType type);
  protected:
    virtual void init(const std::vector<uint8_t>& code);
  protected:
    Device& device;
    VkShaderModule handle;
    StageType type;
  };

  class FileStage : public Stage {
  public:
    FileStage(
      Device& device,
      StageType type,
      const std::string& filePath
    );
    ~FileStage() = default;
    FileStage(const FileStage&) = delete;
    FileStage& operator=(const FileStage&) = delete;

    FileStage(FileStage&& other) = default;
    FileStage& operator=(FileStage&& other) = default;

    const std::string& getFilePath() const { return this->filePath; }

    static constexpr std::string_view shadersPath = "assets/shaders/";
    static std::string ConstructFilePath(
      StageType type,
      const std::string& path
    );
  private:
    std::string filePath;
  };

  class BuiltinStage : public FileStage {
  public:
    BuiltinStage(
      Device& device,
      StageType type,
      std::string_view id
    );
    ~BuiltinStage() = default;
    BuiltinStage(const BuiltinStage&) = delete;
    BuiltinStage& operator=(const BuiltinStage&) = delete;

    BuiltinStage(BuiltinStage&& other) = default;
    BuiltinStage& operator=(BuiltinStage&& other) = default;

    static std::string ConstructId(
      StageType type,
      std::string_view id
    );
  };
};