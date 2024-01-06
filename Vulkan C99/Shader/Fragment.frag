#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 FragColor;
layout(location = 1) in vec2 FragTexCoord;
layout(location = 2) in vec3 FragNormal;
layout(location = 3) in vec3 FragLightPos;
layout(location = 4) in float FragFogVisibility;
layout(location = 5) in vec3 FragViewPos;
layout(location = 6) in vec3 FragClearColor;
layout(location = 7) in vec4 FragShadowCoord;

layout(set = 1, binding = 0) uniform sampler2D Sampler;
layout(set = 2, binding = 0) uniform sampler2D DepthSampler;

layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutBrightColor;

layout(push_constant) uniform PushConstants
{
	layout(offset = 192) bool UseTextures;
	layout(offset = 196) float LightBrightness;
	layout(offset = 200) int FilterLevel;
} PushConts;

bool InShadow = false;
#define Ambient 0.1

float TextureProjection(vec4 ShadowCoord, vec2 Offset)
{
	float CosTheta = clamp(dot(normalize(FragNormal), normalize(FragLightPos)), 0,1);
	float Bias = 0.001 * tan(acos(CosTheta));
	Bias = clamp(Bias, 0, 0.0005);

//	Bias = max(0.05 * (1.0 - dot(FragLightPos, FragNormal)), 0.005);
//	Bias = 0.0005;

	float Visibility = 1.0;
	if (texture(DepthSampler, ShadowCoord.xy + Offset).r < ShadowCoord.z - Bias)
	{
		InShadow = true;
		Visibility = Ambient;
	}
	else
		InShadow = false;

	return Visibility;
}

float FilterPCF(vec4 ShadowCoord)
{
	ivec2 TextureDimension = textureSize(DepthSampler, 0);
	float Scale = 2.0;
	float DX = Scale / float(TextureDimension.x);
	float DY = Scale / float(TextureDimension.y);

	float ShadowFactor = 0.0;
	int Count = 0;
//	int Range = 0;
	int Range = PushConts.FilterLevel;		

	for (int x = -Range; x <= Range; x++)
	{
		for (int y = -Range; y <= Range; y++)
		{
			ShadowFactor += TextureProjection(ShadowCoord, vec2(DX * x, DY * y));
			Count++;
		}
	}

	return ShadowFactor / Count;
}

void main() 
{	
	float Shadow;

	if (PushConts.FilterLevel > 0)
		Shadow = FilterPCF(FragShadowCoord / FragShadowCoord.w);
	else
		Shadow = TextureProjection(FragShadowCoord / FragShadowCoord.w, vec2(0.0));

	vec3 N = normalize(FragNormal);
	vec3 V = normalize(FragViewPos);
	vec3 L = normalize(FragLightPos);
	vec3 R = reflect(-L, N);
	
	vec3 Diffuse = vec3(max(dot(N, L), 0.0)) * PushConts.LightBrightness;
	vec3 Specular = pow(max(dot(R, V), 0.0), 16.0) * vec3(3.35);
		
	if (!InShadow)
		Diffuse += Specular;

	OutColor = vec4((Diffuse * FragColor * Shadow) + FragColor * Ambient, 1.0);

	if (PushConts.UseTextures)
	{
		OutColor *= texture(Sampler, FragTexCoord);
		OutColor.a = texture(Sampler, FragTexCoord).a;
	}

	float Brightness = dot(OutColor.rgb, vec3(0.2126, 0.7152, 0.0722));
   
	if (Brightness > 1.0)
		OutBrightColor = vec4(OutColor.rgb, 1.0);
	else
		OutBrightColor = vec4(0.0, 0.0, 0.0, 1.0);

//	OutColor = mix(vec4(FragClearColor, 1.0), OutColor, FragFogVisibility);
}