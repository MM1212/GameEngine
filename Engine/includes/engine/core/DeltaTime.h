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

    float operator+(const DeltaTime& other) const {
      return this->time + other.time;
    }
    float operator-(const DeltaTime& other) const {
      return this->time - other.time;
    }
    float operator*(const DeltaTime& other) const {
      return this->time * other.time;
    }
    float operator/(const DeltaTime& other) const {
      return this->time / other.time;
    }
    float operator+(float other) const {
      return this->time + other;
    }
    float operator-(float other) const {
      return this->time - other;
    }
    float operator*(float other) const {
      return this->time * other;
    }
    float operator/(float other) const {
      return this->time / other;
    }
  private:
    float time;
  };
}