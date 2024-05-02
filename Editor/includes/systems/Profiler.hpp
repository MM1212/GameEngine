#pragma once

namespace Editor {
  class Profiler {
  public:
    Profiler() = default;
    Profiler(const Profiler&) = default;
    Profiler& operator=(const Profiler&) = default;
    virtual ~Profiler() = default;

    void update(float deltaTime);
  private:
    bool enabled = false;
    float lastOutput = 0.f;
  };
}