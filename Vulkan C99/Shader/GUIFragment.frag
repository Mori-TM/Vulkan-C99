#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D Sampler;
layout(set = 1, binding = 0) uniform sampler2D DepthSampler;
layout(set = 2, binding = 0) uniform sampler2D SceneSampler;
layout(set = 2, binding = 1) uniform sampler2D SceneSampler2;
layout(set = 3, binding = 0) uniform sampler2D BluredSceneSampler;

layout (location = 0) out vec4 OutColor;

struct Color
{
	float r;
	float g;
	float b;
	float a;
};

layout(push_constant) uniform PushConstants
{
	layout(offset = 192) int Display;
	layout(offset = 196) Color Color;
} PushConts;

float LinearizeDepth(float depth)
{
	float N = 1.0;
	float F = 100.0;
	float Z = depth;
	return (2.0 * N) / (F + N - Z * (F - N));	
}

const float Exposure = 1.0;
const float Gamma = 2.2;
const float Smoothing = 1.0/2.0;

void main() 
{
	switch (PushConts.Display)
	{
		case 1:
			OutColor = vec4(PushConts.Color.r, PushConts.Color.g, PushConts.Color.b, texture(Sampler, FragTexCoord).a);
			break;

		case 2:
			OutColor = vec4(PushConts.Color.r, PushConts.Color.g, PushConts.Color.b, PushConts.Color.a);
			break;

		case 3:
			vec3 HdrColor = texture(SceneSampler, FragTexCoord).rgb;      
			vec3 BloomColor = texture(BluredSceneSampler, FragTexCoord).rgb;
			HdrColor += BloomColor; // additive blending
		//	vec3 Result = HdrColor;
			vec3 Result = vec3(1.0) - exp(-HdrColor * Exposure);
   	
		//  Result = pow(Result, vec3(1.0 / Gamma));
			OutColor = vec4(Result, 1.0);

		//	OutColor.rgb = pow(OutColor.rgb, vec3(2.2));

		//	OutColor = texture(BluredSceneSampler, FragTexCoord);
			
			
		//	OutColor.rgb = (OutColor.rgb - 0.5) * (1.0 + 0.05) + 0.5;
			break;

		case 4:
			OutColor = texture(BluredSceneSampler, FragTexCoord);
			break;

		case 5:
			float Depth = LinearizeDepth(texture(DepthSampler, FragTexCoord).r);
			OutColor = vec4(vec3(Depth), 1.0);
			break;
			
		case 6:
			OutColor = texture(SceneSampler, FragTexCoord);
			break;

		case 7:
			OutColor = vec4(vec3(texture(DepthSampler, FragTexCoord).r), 1.0);
			break;
		
		//Font Rendering
		case 8:
			float Distance = texture(Sampler, vec2(PushConts.Color.r, PushConts.Color.g) + (FragTexCoord * 0.0625)).a;
			float Alpha = smoothstep(0.5 - Smoothing, 0.5 + Smoothing, Distance);
			OutColor = vec4(1.0, 1.0, 1.0, Alpha);
			break;

		case 9:
			OutColor = texture(Sampler, FragTexCoord);
			
			break;
	}
}