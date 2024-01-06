#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InColor;
layout(location = 2) in vec2 InTexCoord;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTexCoord;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) mat4 Model;
	layout(offset = 64) mat4 View;
	layout(offset = 128) mat4 Proj;
} PushConts;

void main() 
{
	gl_Position = PushConts.Proj * PushConts.View * PushConts.Model * vec4(InPosition, 1.0);
	FragColor = InColor;
	FragTexCoord = InTexCoord;
}