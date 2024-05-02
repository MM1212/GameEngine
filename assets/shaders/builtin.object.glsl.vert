#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 view;
  mat4 projection;
  mat4 viewProjection;
} gUbo;

layout(push_constant) uniform PushConstants {
  mat4 model;
} pushConsts;

void main() {
  gl_Position = gUbo.viewProjection * pushConsts.model * vec4(position, 1.0);
  fragColor = color;
}