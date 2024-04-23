#pragma once

namespace Engine {
  class DeltaTime {
  public:
    DeltaTime(float time = 0.0f) : time(time) {}

    operator float() const {
      return this->time;
    }
    float asSeconds() const {
      return this->time;
    }
    float asMilliseconds() const {
      return this->time * 1000.0f;
    }
  private:
    float time;
  };
}