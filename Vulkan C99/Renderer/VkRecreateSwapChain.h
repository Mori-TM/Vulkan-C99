void RecreateSwapChain()
{
	vkDeviceWaitIdle(Device);

	CleanUpSwapChain();

	OffScreenPass[0].Width = SHADOW_MAP_WIDTH;
	OffScreenPass[0].Height = SHADOW_MAP_HEIGHT;

	OffScreenPass[1].Width = (Width - 400);
	OffScreenPass[1].Height = Height;

	OffScreenPass[2].Width = (Width - 400);
	OffScreenPass[2].Height = (Height);

	OffScreenPass[3].Width = (Width - 400);
	OffScreenPass[3].Height = (Height);

	if (CreateSwapChain() != VK_SUCCESS)
		printf("Failed to Recreate SwapChain\n");

	if (CreateImageViews() != VK_SUCCESS)
		printf("Failed to Recreate Image Views\n");

	if (CreateSwapChainRenderPass() != VK_SUCCESS)
		printf("Failed to Recreate SapChain Render Pass\n");

	if (CreateOffScreenRenderPasses() != VK_SUCCESS)
		printf("Failed to Create Off Screen Render Pass\n");

	if (CreateOffScreenGraphicsPipelines() != VK_SUCCESS)
		printf("Failed to Recreate Off Screen Graphics Pipeline\n");

	if (CreateGUIGraphicsPipeline() != VK_SUCCESS)
		printf("Failed to Recreate GUI Graphics Pipeline\n");

	if (CreateColorResources() != VK_SUCCESS)
		printf("Failed to Recreate Color Resources\n");

	if (CreateDepthResources() != VK_SUCCESS)
		printf("Failed to Create Depth Resources\n");

	if (CreateSwapChainFramebuffers() != VK_SUCCESS)
		printf("Failed to Recreate SwapChain Framebuffers\n");
	
	if (CreateOffScreenFramebuffers() != VK_SUCCESS)
		printf("Failed to Recreate Off Screen Framebuffers\n");

	if (CreateUniformBuffers() != VK_SUCCESS)
		printf("Failed to Recreate Uniform Buffers\n");

	if (CreateDescriptorPool() != VK_SUCCESS)
		printf("Failed to Recreate Descriptor Pool\n");

	if (CreateUniformDescriptorSets() != VK_SUCCESS)
		printf("Failed to Recreate Uniform Descriptor Set\n");

	if (CreateTextureDescriptorSets() != VK_SUCCESS)
		printf("Failed to Recreate Texture Descriptor Set\n");

	if (CreateCommandBuffers() != VK_SUCCESS)
		printf("Failed to Recreate Command Buffers\n");
}