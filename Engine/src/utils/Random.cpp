#include "utils/Random.h"

using Engine::Random;

std::random_device Random::rd;
std::mt19937 Random::gen(Random::rd());

float Random::Float() {
  return Random::Float(0.0f, 1.0f);
}
float Random::Float(float min, float max) {
  std::uniform_real_distribution<float> dist(min, max);
  return dist(Random::gen);
}
glm::vec2 Random::Vec2() {
  return Random::Vec2(0.0f, 1.0f);
}
glm::vec2 Random::Vec2(float min, float max) {
  return glm::vec2(Random::Float(min, max), Random::Float(min, max));
}
glm::vec3 Random::Vec3() {
  return Random::Vec3(0.0f, 1.0f);
}
glm::vec3 Random::Vec3(float min, float max) {
  return glm::vec3(Random::Float(min, max), Random::Float(min, max), Random::Float(min, max));
}
glm::vec4 Random::Vec4() {
  return Random::Vec4(0.0f, 1.0f);
}
glm::vec4 Random::Vec4(float min, float max) {
  return glm::vec4(Random::Float(min, max), Random::Float(min, max), Random::Float(min, max), Random::Float(min, max));
}