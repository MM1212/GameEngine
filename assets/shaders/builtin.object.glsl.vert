#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 view;
  mat4 projection;
  mat4 viewProjection;
} gUbo;

void main() {
  gl_Position = gUbo.viewProjection * vec4(position, 1.0);
}