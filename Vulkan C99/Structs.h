typedef struct
{
	unsigned int GraphicsFamily;
	unsigned int PresentFamily;
} QueueFamilyIndices;

typedef struct
{
	VkSurfaceCapabilitiesKHR Capabilities;
	VkSurfaceFormatKHR* Formats;
	unsigned int FormatCount;
	VkPresentModeKHR* PresentModes;
	unsigned int PresentModeCount;
} SwapChainSupportDetails;

typedef struct
{
	vec3 Pos;
	vec3 Color;
	vec2 TexCoord;
	vec3 Normal;
} VertexCore;

typedef struct
{
	unsigned int Size;
	VertexCore* Vertex;
} Vertex;

typedef struct
{
	unsigned int Size;
	unsigned int* Index;
} Index;

typedef struct
{
	mat4 LightMat;
	vec4 LightPos;
	vec4 ClearColor;
} UniformBufferObject;

typedef struct
{
	mat4 Model;
	mat4 View;
	mat4 Proj;
} VertexPushConstant;

typedef struct
{
	bool UseTextures;
	float LightBrightness;
	int FilterLevel;
} FragmentPushConstant;

typedef struct
{
	int Display;
	vec4 Color;
} GUIFragmentPushConstant;

typedef struct
{
	bool Horizontal;
} BlurFragmentPushConstant;

//0 = Depth Scene
//1 = 2 Color Normal Scene
//2 = Blur 0
//3 = Blur 1
struct
{
	VkRenderPass RenderPass;
	VkPipeline GraphicsPipeline;
	VkPipelineLayout PipelineLayout;
	VkFramebuffer* Framebuffers;

	VkDescriptorSet DescriptorSet;

	VkImage DepthImage;
	VkDeviceMemory DepthImageMemory;
	VkImageView DepthImageView;

	VkImage ColorImage[2];
	VkDeviceMemory ColorImageMemory[2];
	VkImageView ColorImageView[2];

	VkImage MsaaColorImage[2];
	VkDeviceMemory MsaaColorImageMemory[2];
	VkImageView MsaaColorImageView[2];

	VkSampler Sampler;

	VkDescriptorSetLayout DescriptorSetLayout;

	int Width;
	int Height;
} OffScreenPass[4];

UniformBufferObject UBO;

VertexPushConstant VertexConst;
FragmentPushConstant FragmentConst;
GUIFragmentPushConstant GUIFragmentConst;
BlurFragmentPushConstant BlurFragmentConst;

Vertex ModelVertices;

Vertex QuadVertices3D;
Index QuadIndices3D;

Vertex SphereVertices;
Index SphereIndices;