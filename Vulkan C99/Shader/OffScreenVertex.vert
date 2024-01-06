#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 InPosition;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) mat4 Model;
	layout(offset = 64) mat4 View;
	layout(offset = 128) mat4 Proj;
} PushConts;

void main()
{
	gl_Position = PushConts.Proj * PushConts.View * PushConts.Model * vec4(InPosition, 1.0);
}