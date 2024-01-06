#include "Renderer/VkInstance.h"
#include "Renderer/VkSurface.h"
#include "Renderer/VkPhysicalDevice.h"
#include "Renderer/VkLogicalDevice.h"
#include "Renderer/VkSwapChain.h"
#include "Renderer/VkImageViews.h"
#include "Renderer/VkRenderPass.h"
#include "Renderer/VkDescriptorSetLayout.h"
#include "Renderer/VkGraphicsPipeline.h"
#include "Renderer/VkFrameBuffers.h"
#include "Renderer/VkCommandPool.h"
#include "Renderer/VkColorResources.h"
#include "Renderer/VkDepthResources.h"
#include "Renderer/VkTextureImage.h"
#include "Renderer/VkVertexBuffer.h"
#include "Renderer/VkIndexBuffer.h"
#include "Renderer/VkUniformBuffers.h"
#include "Renderer/VkDescriptorPool.h"
#include "Renderer/VkDescriptorSets.h"
#include "Renderer/VkCommandBuffers.h"
#include "Renderer/VkSemaphores.h"
#include "Renderer/VkRecreateSwapChain.h"
#include "Renderer/VkUpdateUniformBuffers.h"
#include "Renderer/VkRecordCommandBuffer.h"
#include "Renderer/VkDrawFrame.h"

void InitVulkan()
{
	OffScreenPass[0].Width = SHADOW_MAP_WIDTH;
	OffScreenPass[0].Height = SHADOW_MAP_HEIGHT;

	OffScreenPass[1].Width = (Width - 400);
	OffScreenPass[1].Height = Height;

	OffScreenPass[2].Width = (Width - 400);
	OffScreenPass[2].Height = (Height);

	OffScreenPass[3].Width = (Width - 400);
	OffScreenPass[3].Height = (Height);

	if (CreateInstance() != VK_SUCCESS)
		printf("Failed to CreateInstance\n");

	if (CreateSurface() != VK_SUCCESS)
		printf("Failed To Create Surface\n");

	if (PickPhysicalDevice() != VK_SUCCESS)
		printf("Failed to Find Vulkan Supported Graphics Card\n");

	if (CreateLogicalDevice() != VK_SUCCESS)
		printf("Failed to Create Logical Device\n");

	if (CreateSwapChain() != VK_SUCCESS)
		printf("Failed to Create SwapChain\n");

	if (CreateImageViews() != VK_SUCCESS)
		printf("Failed to Create Image Views\n");

	if (CreateSwapChainRenderPass() != VK_SUCCESS)
		printf("Failed to Create SwapChain Render Pass\n");

	if (CreateOffScreenRenderPasses() != VK_SUCCESS)
		printf("Failed to Create Off Screen Render Pass\n");

	if (CreateDescriptorSetLayouts() != VK_SUCCESS)
		printf("Failed to Create Descriptor Set Layout\n");

	if (CreateOffScreenGraphicsPipelines() != VK_SUCCESS)
		printf("Failed to Create Off Screen Graphics Pipeline\n");

	if (CreateGUIGraphicsPipeline() != VK_SUCCESS)
		printf("Failed to Create GUI Graphics Pipeline\n");

	if (CreateCommandPool() != VK_SUCCESS)
		printf("Failed to Create Command Pool\n");

	if (CreateColorResources() != VK_SUCCESS)
		printf("Failed to Create Color Resources\n");

	if (CreateDepthResources() != VK_SUCCESS)
		printf("Failed to Create Depth Resources\n");

	if (CreateSwapChainFramebuffers() != VK_SUCCESS)
		printf("Failed to Create SwapChain Framebuffers\n");

	if (CreateOffScreenFramebuffers() != VK_SUCCESS)
		printf("Failed to Create Off Screen Framebuffers\n");

	if (CreateUniformBuffers() != VK_SUCCESS)
		printf("Failed to Create Uniform Buffers\n");

	if (CreateDescriptorPool() != VK_SUCCESS)
		printf("Failed to Create Descriptor Pool\n");

	if (CreateUniformDescriptorSets() != VK_SUCCESS)
		printf("Failed to Create Uniform Descriptor Set\n");

	if (CreateCommandBuffers() != VK_SUCCESS)
		printf("Failed to Create Command Buffers\n");

	if (CreateSemaphores() != VK_SUCCESS)
		printf("Failed to Create Semaphoren\n");
}

void DestroyVulkan()
{
	vkDeviceWaitIdle(Device);

	CleanUpSwapChain();

//	vkDestroyPipeline(Device, OffScreenPass[0].GraphicsPipeline, NULL);
//	vkDestroyPipeline(Device, OffScreenPass[1].GraphicsPipeline, NULL);
//	vkDestroyPipeline(Device, OffScreenPass[2].GraphicsPipeline, NULL);
//	vkDestroyPipeline(Device, OffScreenPass[3].GraphicsPipeline, NULL);
//	vkDestroyPipeline(Device, GUIGraphicsPipeline, NULL);
//	vkDestroyPipelineLayout(Device, OffScreenPass[0].PipelineLayout, NULL);
//	vkDestroyPipelineLayout(Device, OffScreenPass[1].PipelineLayout, NULL);
//	vkDestroyPipelineLayout(Device, OffScreenPass[2].PipelineLayout, NULL);
//	vkDestroyPipelineLayout(Device, OffScreenPass[3].PipelineLayout, NULL);
//	vkDestroyPipelineLayout(Device, GUIPipelineLayout, NULL);

	for (unsigned int i = 0; i < TextureImageCount; i++)
	{
		vkDestroySampler(Device, TextureSampler[i], NULL);
		vkDestroyImageView(Device, TextureImageViews[i], NULL);

		vkDestroyImage(Device, TextureImages[i], NULL);
		vkFreeMemory(Device, TextureImageMemory[i], NULL);
	}

	vkDestroyDescriptorSetLayout(Device, UniformDescriptorSetLayout, NULL);
	vkDestroyDescriptorSetLayout(Device, TextureDescriptorSetLayout, NULL);
	vkDestroyDescriptorSetLayout(Device, OffScreenPass[0].DescriptorSetLayout, NULL);
	vkDestroyDescriptorSetLayout(Device, OffScreenPass[1].DescriptorSetLayout, NULL);
	vkDestroyDescriptorSetLayout(Device, OffScreenPass[2].DescriptorSetLayout, NULL);
	vkDestroyDescriptorSetLayout(Device, OffScreenPass[3].DescriptorSetLayout, NULL);

	SandboxDeInit();

	for (unsigned int i = 0; i < VertexBufferMemoryCount; i++)
		vkFreeMemory(Device, VertexBufferMemory[i], NULL);
	for (unsigned int i = 0; i < IndexBufferMemoryCount; i++)
		vkFreeMemory(Device, IndexBufferMemory[i], NULL);

	for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(Device, ImageAvailableSemaphores[i], NULL);
		vkDestroySemaphore(Device, RenderFinishedSemaphores[i], NULL);
		vkDestroyFence(Device, InFlightFences[i], NULL);
	}

	vkDestroyCommandPool(Device, CommandPool, NULL);

	vkDestroyDevice(Device, NULL);
	vkDestroySurfaceKHR(Instance, Surface, NULL);
	vkDestroyInstance(Instance, NULL);
}