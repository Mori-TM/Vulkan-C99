#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec3 InColor;
layout(location = 2) in vec2 InTexCoord;
layout(location = 3) in vec3 InNormal;

layout(location = 0) out vec3 FragColor;
layout(location = 1) out vec2 FragTexCoord;
layout(location = 2) out vec3 FragNormal;
layout(location = 3) out vec3 FragLightPos;
layout(location = 4) out float FragFogVisibility;
layout(location = 5) out vec3 FragViewPos;
layout(location = 6) out vec3 FragClearColor;
layout(location = 7) out vec4 FragShadowCoord;

layout(set = 0, binding = 0) uniform UniformBufferObject
{
	mat4 LightMat;
	vec4 LightPos;
	vec4 ClearColor;
} UBO;

layout(push_constant) uniform PushConstants
{
	layout(offset = 0) mat4 Model;
	layout(offset = 64) mat4 View;
	layout(offset = 128) mat4 Proj;
} PushConts;

const float FogGradient = 1.0;
const float FogDensity = 0.06;

const mat4 BiasMat = mat4
( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

void main() 
{
	vec4 WorldPos = PushConts.Model * vec4(InPosition, 1.0);
	vec4 PosRelToCam = PushConts.View * WorldPos;

	gl_Position = PushConts.Proj * PosRelToCam;

	FragColor = InColor;
	FragTexCoord = InTexCoord;
    FragNormal = mat3(PushConts.View) * mat3(PushConts.Model) * InNormal;
    FragLightPos = mat3(PushConts.View) * (UBO.LightPos.xyz - WorldPos.xyz);
    FragViewPos = -(PushConts.View * WorldPos).xyz;		
	FragClearColor = UBO.ClearColor.xyz;
	FragShadowCoord = (BiasMat * UBO.LightMat * PushConts.Model) * vec4(InPosition, 1.0);	
	
	float Distance = length(PosRelToCam.xyz);
    float Visibility = exp(-pow((Distance * FogDensity), FogGradient)) * 1.4;
    FragFogVisibility = clamp(Visibility, 0.0, 1.0);
}