#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertColor;
layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = vec4(vertColor.r + 0.5, vertColor.g + 0.5, vertColor.b + 0.5, 1.0);
}