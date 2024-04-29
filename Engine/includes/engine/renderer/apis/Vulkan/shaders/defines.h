#pragma once

#include "renderer/RendererAPI.h"
#include "renderer/apis/Vulkan/defines.h"
#include "renderer/apis/Vulkan/Device.h"
#include "renderer/apis/Vulkan/CommandBuffer.h"
#include "renderer/apis/Vulkan/Pipeline.h"
#include "base/Stage.h"

#include <string_view>
#include <vector>

namespace Engine::Renderers::Vulkan::Shaders {
  class Base;

  class Base {
  public:
    Base(Device& device, const std::string& name);
    Base(Device& device, std::string_view name);
    virtual ~Base();

    Base(const Base&) = delete;
    Base& operator=(const Base&) = delete;

    virtual void use(CommandBuffer& cmdBuffer) = 0;
    std::string_view getName() const { return this->name; }
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
    Ref<T> addStage(Args&&... args) {
      Ref<T> stage = MakeRef<T>(this->device, *this, std::forward<Args>(args)...);
      this->stages.push_back(stage);
      return stage;
    }
    static std::vector<uint8_t> ReadFile(const std::string_view filePath);
  protected:
    virtual void init(const Pipeline::ConfigInfo& pipelineConfigInfo);
    friend class Pipeline;
    friend class Stage;
  protected:
    Device& device;
    std::vector<Ref<Stage>> stages;
    std::string name;
    Scope<Pipeline> pipeline = nullptr;
  };
};