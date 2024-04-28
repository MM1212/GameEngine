#pragma once

#include "renderer/apis/Vulkan/defines.h"
#include "renderer/apis/Vulkan/Device.h"
#include "renderer/apis/Vulkan/CommandBuffer.h"
#include "base/Pipeline.h"
#include "base/Stage.h"

#include <string_view>
#include <vector>

namespace Engine::Renderers::Vulkan::Shaders {
  class Base;

  class Base {
  public:
    Base(Device& device, const Pipeline::ConfigInfo& pipelineConfigInfo);
    virtual ~Base() = default;

    Base(const Base&) = delete;
    Base& operator=(const Base&) = delete;

    void use();
    const Ref<Stage> getStage(StageType type) const {
      auto it = std::find_if(
        this->stages.begin(),
        this->stages.end(),
        [type](const Ref<Stage>& stage) {
          return stage->getType() == type;
        }
      );
      if (it == this->stages.end())
        return nullptr;
      return *it;
    }
    template <typename T, typename ...Args>
    void addStage(Args&&... args) {
      this->stages.push_back(MakeRef<T>(this->device, std::forward<Args>(args)...));
    }
    static std::vector<uint8_t> ReadFile(const std::string_view filePath);
  protected:
    virtual void init() = 0;
    friend class Pipeline;
    friend class Stage;
  protected:
    Device& device;
    std::vector<Ref<Stage>> stages;
    // Pipeline pipeline;
  };
};