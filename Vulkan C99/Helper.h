unsigned int ValidationLayerCount = 1;
const char* ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };
const char* DeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
VkInstance Instance;
VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
VkDevice Device;
VkQueue GraphicsQueue;
VkQueue PresentQueue;
VkSurfaceKHR Surface;

VkSwapchainKHR SwapChain;
VkImage* SwapChainImages;
VkFormat SwapChainImageFormat;
VkExtent2D SwapChainExtent;
unsigned int SwapChainImageViewCount = 0;
VkImageView* SwapChainImageViews;

VkRenderPass SwapChainRenderPass;
VkDescriptorSetLayout UniformDescriptorSetLayout;
VkDescriptorSetLayout TextureDescriptorSetLayout;

VkPipelineLayout GUIPipelineLayout;
VkPipeline GUIGraphicsPipeline;

VkFramebuffer* SwapChainFramebuffers;

VkCommandPool CommandPool;
VkCommandBuffer* CommandBuffers;

VkSemaphore ImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
VkSemaphore RenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
VkFence InFlightFences[MAX_FRAMES_IN_FLIGHT];
VkFence* InFlightImages;

unsigned int CurrentFrame = 0;
unsigned int ImageIndex;

VkBuffer ModelVertexBuffer;

VkBuffer QuadVertexBuffer3D;
VkBuffer QuadIndexBuffer3D;

VkBuffer SphereVertexBuffer;
VkBuffer SphereIndexBuffer;

VkBuffer UniformBuffer;

unsigned int VertexBufferMemoryCount = 0;
VkDeviceMemory* VertexBufferMemory;
unsigned int IndexBufferMemoryCount = 0;
VkDeviceMemory* IndexBufferMemory;
VkDeviceMemory UniformBufferMemory;

VkDescriptorPool DescriptoPool;
VkDescriptorSet UniformDescriptorSet;

unsigned int TextureDescriptorSetsCount = 0;
VkDescriptorSet* TextureDescriptorSets;

unsigned int MipLevels;

unsigned int TextureImageCount = 0;
VkImage* TextureImages;
VkDeviceMemory* TextureImageMemory;
VkImageView* TextureImageViews;
VkSampler* TextureSampler;

VkImage DepthImage;
VkDeviceMemory DepthImageMemory;
VkImageView DepthImageView;

VkSampleCountFlagBits MsaaSamples = VK_SAMPLE_COUNT_8_BIT;

vec4 ClearColor = { 0.01, 0.01, 0.01, 1.0 };

vec3 CameraPos = { 0.0, 5.0, -3.0 };
vec3 CameraFront = { 0.0, 1.0, 1.0 };
vec3 CameraUp = { 0.0, 1.0, 0.0 };

bool LoadModel = false;
bool LoadTexture = false;

struct timespec TimerStartTime;
struct timespec TimerEndTime;

double TimeToRenderDepth = 0.0;
double TimeToRenderScene = 0.0;
double TimeToRenderBlur = 0.0;
double TimeToRenderGUI = 0.0;
double TimeToSubmitQueue = 0.0;
double TimeToPresentQueue = 0.0;

SDL_Window* Window;
SDL_Event Event;

int Width = 1600;
int Height = 900;

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice PhysicalDevice)
{
	unsigned int QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, NULL);

	VkQueueFamilyProperties* QueueFamilies = malloc(QueueFamilyCount * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies);

	QueueFamilyIndices Indices;

	for (unsigned int i = 0; i < QueueFamilyCount; i++)
	{
		if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			Indices.GraphicsFamily = i;

		VkBool32 Supported = VK_FALSE;
		VkResult Res = vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &Supported);
		
		if (Supported)
			Indices.PresentFamily = i;

		if (Indices.GraphicsFamily != NULL && Indices.PresentFamily != NULL)
			break;
	}

	return Indices;
}

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice PhysicalDevice)
{
	SwapChainSupportDetails Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &Details.Capabilities);

	unsigned int FormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, NULL);
	Details.Formats = malloc(FormatCount * sizeof(VkSurfaceFormatKHR));
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, Details.Formats);
	Details.FormatCount = FormatCount;

	unsigned int PresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, NULL);
	Details.PresentModes = malloc(PresentModeCount * sizeof(VkPresentModeKHR));
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, Details.PresentModes);
	Details.PresentModeCount = PresentModeCount;

	return Details;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* AvailableFormats, unsigned int FormatCount)
{
	for (unsigned int i = 0; i < FormatCount; i++)
		if (AvailableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return AvailableFormats[i];

	return AvailableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(VkPresentModeKHR* AvailablePresentModes, unsigned int PresentCount)
{
	for (unsigned int i = 0; i < PresentCount; i++)
		if (AvailablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return AvailablePresentModes[i];

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR Capabilities)
{
	if (Capabilities.currentExtent.width != UINT32_MAX)
		return Capabilities.currentExtent;
	else
	{
		int Width = 0;
		int Height = 0;

		SDL_GetWindowSize(Window, &Width, &Height);

		VkExtent2D ActualExtent =
		{
			Width, 
			Height
		};

		ActualExtent.width = MAX(Capabilities.minImageExtent.width, MIN(Capabilities.maxImageExtent.width, ActualExtent.width));
		ActualExtent.height = MAX(Capabilities.minImageExtent.height, MIN(Capabilities.maxImageExtent.height, ActualExtent.height));

		return ActualExtent;
	}
}

VkShaderModule CreateShaderModule(char* Code, size_t Size)
{
	VkShaderModuleCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.codeSize = Size;
	CreateInfo.pCode = Code;

	VkShaderModule ShaderModule;
	if (vkCreateShaderModule(Device, &CreateInfo, NULL, &ShaderModule) != VK_SUCCESS)
		printf("Faild to Create Shader Module\n");

	return ShaderModule;
}

unsigned int FindMemoryType(unsigned int TypeFilter, VkMemoryPropertyFlags Properties)
{
	VkPhysicalDeviceMemoryProperties MemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &MemoryProperties);

	for (unsigned int i = 0; i < MemoryProperties.memoryTypeCount; i++)
		if (TypeFilter & (1 << i) && (MemoryProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
			return i;
	
	printf("Faild To find a Suitable Memory Type\n");
}
VkResult CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer* Buffer, VkDeviceMemory* BufferMemory)
{
	VkBufferCreateInfo BufferInfo;
	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.pNext = NULL;
	BufferInfo.flags = 0;
	BufferInfo.size = Size;
	BufferInfo.usage = Usage;
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferInfo.queueFamilyIndexCount = 0;
	BufferInfo.pQueueFamilyIndices = NULL;

	if (vkCreateBuffer(Device, &BufferInfo, NULL, Buffer) != VK_SUCCESS)
		return 1;

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(Device, *Buffer, &MemoryRequirements);

	VkMemoryAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.allocationSize = MemoryRequirements.size;
	AllocateInfo.memoryTypeIndex = FindMemoryType(MemoryRequirements.memoryTypeBits, Properties);

	if (vkAllocateMemory(Device, &AllocateInfo, NULL, BufferMemory) != VK_SUCCESS)
		return 1;

	vkBindBufferMemory(Device, *Buffer, *BufferMemory, 0);

	return VK_SUCCESS;
}

VkCommandBuffer BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.commandPool = CommandPool;
	AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer;
	vkAllocateCommandBuffers(Device, &AllocateInfo, &CommandBuffer);

	VkCommandBufferBeginInfo BeginInfo;
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = NULL;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = NULL;

	vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

	return CommandBuffer;
}

void EndSingleTimeCommandBuffer(VkCommandBuffer CommandBuffer)
{
	vkEndCommandBuffer(CommandBuffer);

	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 0;
	SubmitInfo.pWaitSemaphores = NULL;
	SubmitInfo.pWaitDstStageMask = NULL;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;
	SubmitInfo.signalSemaphoreCount = 0;
	SubmitInfo.pSignalSemaphores = NULL;

	vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(GraphicsQueue);

	vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
}

VkResult CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
{
	VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

	VkBufferCopy CopyRegion;
	CopyRegion.srcOffset = 0;
	CopyRegion.dstOffset = 0;
	CopyRegion.size = Size;

	vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

	EndSingleTimeCommandBuffer(CommandBuffer);

	return VK_SUCCESS;
}

VkImageView CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags, unsigned int MipLevels)
{
	VkImageViewCreateInfo ViewInfo;
	ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ViewInfo.pNext = NULL;
	ViewInfo.flags = 0;
	ViewInfo.image = Image;
	ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ViewInfo.format = Format;
	ViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	ViewInfo.subresourceRange.aspectMask = AspectFlags;
	ViewInfo.subresourceRange.baseMipLevel = 0;
	ViewInfo.subresourceRange.levelCount = MipLevels;
	ViewInfo.subresourceRange.baseArrayLayer = 0;
	ViewInfo.subresourceRange.layerCount = 1;

	VkImageView ImageView;

	if (vkCreateImageView(Device, &ViewInfo, NULL, &ImageView) != VK_SUCCESS)
		return 0;

	return ImageView;
}

VkResult CreateTextureSampler(VkSampler* TextureSampler, unsigned int MipLevels, VkFilter Filter, VkSamplerAddressMode AddressMode)
{
	VkSamplerCreateInfo SamplerInfo;
	SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	SamplerInfo.pNext = NULL;
	SamplerInfo.flags = 0;
	SamplerInfo.magFilter = Filter;
	SamplerInfo.minFilter = Filter;
	SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	SamplerInfo.addressModeU = AddressMode;
	SamplerInfo.addressModeV = AddressMode;
	SamplerInfo.addressModeW = AddressMode;
	SamplerInfo.mipLodBias = 0.0;
	SamplerInfo.anisotropyEnable = VK_FALSE;
	SamplerInfo.maxAnisotropy = 1;
	SamplerInfo.compareEnable = VK_FALSE;
	SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	SamplerInfo.minLod = 0.0;
	SamplerInfo.maxLod = MipLevels;
	SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	SamplerInfo.unnormalizedCoordinates = VK_FALSE;

	if (vkCreateSampler(Device, &SamplerInfo, NULL, TextureSampler) != VK_SUCCESS)
		return 1;

	return VK_SUCCESS;
}

VkFormat FindSupportedFormat(VkFormat* Candidates, unsigned int CandidatesCount, VkImageTiling Tiling, VkFormatFeatureFlags Features)
{
	for (unsigned int i = 0; i < CandidatesCount; i++)
	{
		VkFormatProperties Properties;
		vkGetPhysicalDeviceFormatProperties(PhysicalDevice, Candidates[i], &Properties);

		if (Tiling == VK_IMAGE_TILING_LINEAR && (Properties.linearTilingFeatures & Features) == Features)
			return Candidates[i];
		else if (Tiling == VK_IMAGE_TILING_OPTIMAL && (Properties.optimalTilingFeatures & Features) == Features)
			return Candidates[i];
	}
}

VkFormat FindDepthFormat()
{
	VkFormat Formats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
	return FindSupportedFormat(Formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkSampleCountFlags GetMaxSampleCount()
{
	VkPhysicalDeviceProperties PhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

	VkSampleCountFlags SampleCount = PhysicalDeviceProperties.limits.framebufferColorSampleCounts & PhysicalDeviceProperties.limits.framebufferDepthSampleCounts;

	if (SampleCount & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (SampleCount & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (SampleCount & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (SampleCount & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (SampleCount & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (SampleCount & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

void BindVertices(VkBuffer* VertexBuffer, unsigned int DescriptorPos, VkPipelineLayout* PipelineLayout, bool BindDescriptorSets)
{
	VkDeviceSize Offsets[] = { 0 };
	vkCmdBindVertexBuffers(CommandBuffers[ImageIndex], 0, 1, VertexBuffer, Offsets);
	if (BindDescriptorSets)
	{
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 0, 1, &UniformDescriptorSet, 0, NULL);
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 1, 1, &TextureDescriptorSets[DescriptorPos], 0, NULL);
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 2, 1, &OffScreenPass[0].DescriptorSet, 0, NULL);
	}	
}

void DrawVertices(Vertex* Vertices, VkPipelineLayout* PipelineLayout)
{
	vkCmdPushConstants(CommandBuffers[ImageIndex], *PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, VERTEX_OFFSET, sizeof(VertexPushConstant), &VertexConst);
	vkCmdDraw(CommandBuffers[ImageIndex], Vertices->Size, 1, 0, 0);
}

void BindIndices(VkBuffer* VertexBuffer, VkBuffer* IndexBuffer, unsigned int DescriptorPos, VkPipelineLayout* PipelineLayout, bool BindDescriptorSets)
{
	VkDeviceSize Offsets[] = { 0 };
	vkCmdBindVertexBuffers(CommandBuffers[ImageIndex], 0, 1, VertexBuffer, Offsets);
	vkCmdBindIndexBuffer(CommandBuffers[ImageIndex], *IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	if (BindDescriptorSets)
	{
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 0, 1, &UniformDescriptorSet, 0, NULL);
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 1, 1, &TextureDescriptorSets[DescriptorPos], 0, NULL);
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, *PipelineLayout, 2, 1, &OffScreenPass[0].DescriptorSet, 0, NULL);
	}
}

void DrawIndices(Index* Indices, VkPipelineLayout* PipelineLayout)
{
	vkCmdPushConstants(CommandBuffers[ImageIndex], *PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, VERTEX_OFFSET, sizeof(VertexPushConstant), &VertexConst);
	vkCmdDrawIndexed(CommandBuffers[ImageIndex], Indices->Size, 1, 0, 0, 0);
}

void CleanUpSwapChain()
{
	vkDestroyImageView(Device, DepthImageView, NULL);
	vkDestroyImage(Device, DepthImage, NULL);
	vkFreeMemory(Device, DepthImageMemory, NULL);

	vkDestroyImageView(Device, OffScreenPass[0].DepthImageView, NULL);
	vkDestroyImage(Device, OffScreenPass[0].DepthImage, NULL);
	vkFreeMemory(Device, OffScreenPass[0].DepthImageMemory, NULL);
	vkDestroySampler(Device, OffScreenPass[0].Sampler, NULL);

	vkDestroyImageView(Device, OffScreenPass[1].ColorImageView[0], NULL);
	vkDestroyImage(Device, OffScreenPass[1].ColorImage[0], NULL);
	vkFreeMemory(Device, OffScreenPass[1].ColorImageMemory[0], NULL);

	vkDestroyImageView(Device, OffScreenPass[1].ColorImageView[1], NULL);
	vkDestroyImage(Device, OffScreenPass[1].ColorImage[1], NULL);
	vkFreeMemory(Device, OffScreenPass[1].ColorImageMemory[1], NULL);

	vkDestroyImageView(Device, OffScreenPass[1].DepthImageView, NULL);
	vkDestroyImage(Device, OffScreenPass[1].DepthImage, NULL);
	vkFreeMemory(Device, OffScreenPass[1].DepthImageMemory, NULL);
	vkDestroySampler(Device, OffScreenPass[1].Sampler, NULL);

	vkDestroyImageView(Device, OffScreenPass[1].MsaaColorImageView[0], NULL);
	vkDestroyImage(Device, OffScreenPass[1].MsaaColorImage[0], NULL);
	vkFreeMemory(Device, OffScreenPass[1].MsaaColorImageMemory[0], NULL);

	vkDestroyImageView(Device, OffScreenPass[1].MsaaColorImageView[1], NULL);
	vkDestroyImage(Device, OffScreenPass[1].MsaaColorImage[1], NULL);
	vkFreeMemory(Device, OffScreenPass[1].MsaaColorImageMemory[1], NULL);

	vkDestroyImageView(Device, OffScreenPass[2].ColorImageView[0], NULL);
	vkDestroyImage(Device, OffScreenPass[2].ColorImage[0], NULL);
	vkFreeMemory(Device, OffScreenPass[2].ColorImageMemory[0], NULL);
	vkDestroySampler(Device, OffScreenPass[2].Sampler, NULL);

	vkDestroyImageView(Device, OffScreenPass[3].ColorImageView[0], NULL);
	vkDestroyImage(Device, OffScreenPass[3].ColorImage[0], NULL);
	vkFreeMemory(Device, OffScreenPass[3].ColorImageMemory[0], NULL);
	vkDestroySampler(Device, OffScreenPass[3].Sampler, NULL);

	vkDestroyPipeline(Device, OffScreenPass[0].GraphicsPipeline, NULL);
	vkDestroyPipeline(Device, OffScreenPass[1].GraphicsPipeline, NULL);
	vkDestroyPipeline(Device, OffScreenPass[2].GraphicsPipeline, NULL);
	vkDestroyPipeline(Device, OffScreenPass[3].GraphicsPipeline, NULL);
	vkDestroyPipeline(Device, GUIGraphicsPipeline, NULL);
	vkDestroyPipelineLayout(Device, OffScreenPass[0].PipelineLayout, NULL);
	vkDestroyPipelineLayout(Device, OffScreenPass[1].PipelineLayout, NULL);
	vkDestroyPipelineLayout(Device, OffScreenPass[2].PipelineLayout, NULL);
	vkDestroyPipelineLayout(Device, OffScreenPass[3].PipelineLayout, NULL);
	vkDestroyPipelineLayout(Device, GUIPipelineLayout, NULL);

	for (unsigned int i = 0; i < SwapChainImageViewCount; i++)
	{
		vkDestroyFramebuffer(Device, SwapChainFramebuffers[i], NULL);
		vkDestroyFramebuffer(Device, OffScreenPass[0].Framebuffers[i], NULL);
		vkDestroyFramebuffer(Device, OffScreenPass[1].Framebuffers[i], NULL);
		vkDestroyFramebuffer(Device, OffScreenPass[2].Framebuffers[i], NULL);
		vkDestroyFramebuffer(Device, OffScreenPass[3].Framebuffers[i], NULL);
	}

	vkFreeCommandBuffers(Device, CommandPool, SwapChainImageViewCount, CommandBuffers);

	vkDestroyRenderPass(Device, SwapChainRenderPass, NULL);
	for (unsigned int i = 0; i < 4; i++)
		vkDestroyRenderPass(Device, OffScreenPass[i].RenderPass, NULL);

	for (unsigned int i = 0; i < SwapChainImageViewCount; i++)
		vkDestroyImageView(Device, SwapChainImageViews[i], NULL);

	vkDestroySwapchainKHR(Device, SwapChain, NULL);

	vkDestroyBuffer(Device, UniformBuffer, NULL);
	vkFreeMemory(Device, UniformBufferMemory, NULL);

	vkDestroyDescriptorPool(Device, DescriptoPool, NULL);
}

//Fps
unsigned int LastFPS = 0;
unsigned int CurrentFPS = 0;
unsigned int Frames = 0;
char CharFPS[64];

void CalcFps(const char* title)
{
	Frames++;
	
	if (LastFPS < SDL_GetTicks() - 1000)
	{
		LastFPS = SDL_GetTicks();
		CurrentFPS = Frames;
		Frames = 0;

		sprintf(CharFPS, "%s FPS: %d ms: %.3f", title, CurrentFPS, 1000.0 / CurrentFPS);
	//	SDL_SetWindowTitle(Window, CharFPS);
	}
}

char* ReadFileData(char* Path, size_t* Size)
{
	char* Buffer = 0;
	long Length = 0;

	FILE* File = fopen(Path, "rb");
	if (!File)
		return NULL;

	fseek(File, 0, SEEK_END);
	Length = ftell(File);
	fseek(File, 0, SEEK_SET);
	Buffer = (char*)malloc(Length+1);
	fread(Buffer, sizeof(char), Length, File);

	fclose(File);

	*Size = Length;
	return Buffer;
}

float DeltaTime()
{
	static float DeltaTime = 0.0;
	static float LastTime = 0.0;

	float CurrentFrame = SDL_GetTicks();
	DeltaTime = CurrentFrame - LastTime;
	LastTime = CurrentFrame;

	return DeltaTime;
}

void StartTimer()
{
	timespec_get(&TimerStartTime, TIME_UTC);
}

double EndTimer(const char* Msg)
{
	timespec_get(&TimerEndTime, TIME_UTC);

	double Time = (double)((TimerEndTime.tv_sec) * 1000.0 + (TimerEndTime.tv_nsec) / 1000000.0) - ((TimerStartTime.tv_sec) * 1000.0 + (TimerStartTime.tv_nsec) / 1000000.0);

	return Time;
}

void PrintRenderTimes()
{
	static unsigned int LastTime = 0;

	if (LastTime < SDL_GetTicks() - 700)
	{
		LastTime = SDL_GetTicks();

		system("@cls||clear");

		printf("Time To Render Depth: \t\tMS: %f\n", TimeToRenderDepth);
		printf("Time To Render Scene: \t\tMS: %f\n", TimeToRenderScene);
		printf("Time To Render Blur: \t\tMS: %f\n", TimeToRenderBlur);
		printf("Time To Render GUI: \t\tMS: %f\n", TimeToRenderGUI);
		printf("Time To Submit Queue: \t\tMS: %f\n", TimeToSubmitQueue);
		printf("Time To Present Queue: \t\tMS: %f\n\n", TimeToPresentQueue);
	}
}

void ModelToVertex(WaveModelData* ModelData, Vertex* Vertices)
{
	Vertices->Vertex = malloc(ModelData->Mesh->VerticeCount * sizeof(VertexCore));
	Vertices->Size = ModelData->Mesh->VerticeCount;

	int MaterialIndex = 0;

	for (unsigned int i = 0; i < ModelData->Mesh->VerticeCount; i++)
	{
		Vertices->Vertex[i].Pos.x = ModelData->Mesh[i].Vertices.x;
		Vertices->Vertex[i].Pos.y = ModelData->Mesh[i].Vertices.y;
		Vertices->Vertex[i].Pos.z = ModelData->Mesh[i].Vertices.z;

		Vertices->Vertex[i].TexCoord.x = ModelData->Mesh[i].TexCoords.x;
		Vertices->Vertex[i].TexCoord.y = ModelData->Mesh[i].TexCoords.y;
				
		Vertices->Vertex[i].Normal.x = ModelData->Mesh[i].Normals.x;
		Vertices->Vertex[i].Normal.y = ModelData->Mesh[i].Normals.y;
		Vertices->Vertex[i].Normal.z = ModelData->Mesh[i].Normals.z;
				
		if (ModelData->HasMaterial == 1)
		{
			if (ModelData->Material[MaterialIndex].VertexIndex == i)
				MaterialIndex++;

			Vertices->Vertex[i].Color.x = ModelData->Material[MaterialIndex-1].DiffuseColor.x;
			Vertices->Vertex[i].Color.y = ModelData->Material[MaterialIndex-1].DiffuseColor.y;
			Vertices->Vertex[i].Color.z = ModelData->Material[MaterialIndex-1].DiffuseColor.z;
		}
		else
		{
			Vertices->Vertex[i].Color.x = ModelData->Mesh[i].VertexColor.x;
			Vertices->Vertex[i].Color.y = ModelData->Mesh[i].VertexColor.y;
			Vertices->Vertex[i].Color.z = ModelData->Mesh[i].VertexColor.z;
		}
		
	}

//	WaveWriteWave("C:/Models/Bugatti.wave", ModelData);

	WaveFreeModel(ModelData);
}

void CreateSphere(int SegmentsX, int SegmentsY)
{
	SphereVertices.Vertex = malloc((SegmentsX + 1) * (SegmentsY + 1) * sizeof(VertexCore));
	SphereVertices.Size = 0;

	SphereIndices.Index = malloc((SegmentsX * SegmentsY) * 6 * sizeof(unsigned int));
	SphereIndices.Size = 0;

	for (int y = 0; y <= SegmentsY; y++)
	{
		for (int x = 0; x <= SegmentsX; x++)
		{
			float xSegment = (float)x / (float)SegmentsX;
			float ySegment = (float)y / (float)SegmentsY;
			float xPos = cosf(xSegment * PI2) * sinf(ySegment * PI);
			float yPos = cosf(ySegment * PI);
			float zPos = sinf(xSegment * PI2) * sinf(ySegment * PI);

			SphereVertices.Vertex[SphereVertices.Size].Pos.x = xPos;
			SphereVertices.Vertex[SphereVertices.Size].Pos.y = yPos;
			SphereVertices.Vertex[SphereVertices.Size].Pos.z = zPos;

			SphereVertices.Vertex[SphereVertices.Size].Color.x = 1.0;
			SphereVertices.Vertex[SphereVertices.Size].Color.y = 1.0;
			SphereVertices.Vertex[SphereVertices.Size].Color.z = 1.0;

			SphereVertices.Vertex[SphereVertices.Size].TexCoord.x = xSegment;
			SphereVertices.Vertex[SphereVertices.Size].TexCoord.y = ySegment;

			SphereVertices.Vertex[SphereVertices.Size].Normal.x = xPos;
			SphereVertices.Vertex[SphereVertices.Size].Normal.y = yPos;
			SphereVertices.Vertex[SphereVertices.Size].Normal.z = zPos;
			SphereVertices.Size++;
		}
	}

	for (int y = 0; y < SegmentsY; y++)
	{
		for (int x = 0; x < SegmentsX; x++)
		{
			SphereIndices.Index[SphereIndices.Size] = (y + 1) * (SegmentsX + 1) + x;
			SphereIndices.Size++;
			SphereIndices.Index[SphereIndices.Size] = y * (SegmentsX + 1) + x;
			SphereIndices.Size++;
			SphereIndices.Index[SphereIndices.Size] = y * (SegmentsX + 1) + x + 1;
			SphereIndices.Size++;

			SphereIndices.Index[SphereIndices.Size] = (y + 1) * (SegmentsX + 1) + x;
			SphereIndices.Size++;
			SphereIndices.Index[SphereIndices.Size] = y * (SegmentsX + 1) + x + 1;
			SphereIndices.Size++;
			SphereIndices.Index[SphereIndices.Size] = (y + 1) * (SegmentsX + 1) + x + 1;
			SphereIndices.Size++;
		}
	}
}

void FreeVertex(Vertex* Vertices, Index* Indices)
{
	free(Vertices->Vertex);
	if (Indices)
		free(Indices->Index);
}

void PrintStats(VkPhysicalDevice PhysicalDevice)
{
	VkPhysicalDeviceProperties Properties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &Properties);

	unsigned int ApiVersion = Properties.apiVersion;
	printf("\x1B[31mGrafikkarte: %s\033[0m\t\t\n", Properties.deviceName);
	printf("\x1B[32mAPI version: %d.%d.%d\033[0m\t\t\n", VK_VERSION_MAJOR(ApiVersion), VK_VERSION_MINOR(ApiVersion), VK_VERSION_PATCH(ApiVersion));
	printf("\x1B[34mDriver Version: %d\033[0m\t\t\n", Properties.driverVersion);
}