VkResult CreateSwapChainFramebuffers()
{
	SwapChainFramebuffers = malloc(SwapChainImageViewCount * sizeof(VkFramebuffer));

	for (unsigned int i = 0; i < SwapChainImageViewCount; i++)
	{
		VkImageView Attachments[] =
		{
			SwapChainImageViews[i],
		//	ColorImageView,
			DepthImageView,
			
		};

		VkFramebufferCreateInfo FramebufferInfo;
		FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferInfo.pNext = NULL;
		FramebufferInfo.flags = 0;
		FramebufferInfo.renderPass = SwapChainRenderPass;
		FramebufferInfo.attachmentCount = 2;
		FramebufferInfo.pAttachments = Attachments;
		FramebufferInfo.width = SwapChainExtent.width;
		FramebufferInfo.height = SwapChainExtent.height;
		FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(Device, &FramebufferInfo, NULL, &SwapChainFramebuffers[i]) != VK_SUCCESS)
			return 1;
	}

	return VK_SUCCESS;
}

VkResult CreateOffScreenFramebuffer(unsigned int Index, unsigned int AttachmentCount, VkImageView* Attachments, VkSamplerAddressMode SamplerMode)
{
	OffScreenPass[Index].Framebuffers = malloc(SwapChainImageViewCount * sizeof(VkFramebuffer));

	if (CreateTextureSampler(&OffScreenPass[Index].Sampler, 1.0, VK_FILTER_NEAREST, SamplerMode) != VK_SUCCESS)
		return 1;

	for (unsigned int i = 0; i < SwapChainImageViewCount; i++)
	{
		VkFramebufferCreateInfo FramebufferInfo;
		FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferInfo.pNext = NULL;
		FramebufferInfo.flags = 0;
		FramebufferInfo.renderPass = OffScreenPass[Index].RenderPass;
		FramebufferInfo.attachmentCount = AttachmentCount;
		FramebufferInfo.pAttachments = Attachments;
		FramebufferInfo.width = OffScreenPass[Index].Width;
		FramebufferInfo.height = OffScreenPass[Index].Height;
		FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(Device, &FramebufferInfo, NULL, &OffScreenPass[Index].Framebuffers[i]) != VK_SUCCESS)
			return 1;
	}

	return VK_SUCCESS;
}

VkResult CreateOffScreenFramebuffers()
{
	VkImageView Attachments[] =
	{
		OffScreenPass[1].MsaaColorImageView[0],
		OffScreenPass[1].MsaaColorImageView[1],
			
		OffScreenPass[1].DepthImageView,

		OffScreenPass[1].ColorImageView[0],
		OffScreenPass[1].ColorImageView[1],
	};

	if (CreateOffScreenFramebuffer(0, 1, &OffScreenPass[0].DepthImageView, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER) != VK_SUCCESS) return 1;
	if (CreateOffScreenFramebuffer(1, ARRAYSIZE(Attachments), Attachments, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE) != VK_SUCCESS) return 1;
	if (CreateOffScreenFramebuffer(2, 1, &OffScreenPass[2].ColorImageView[0], VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE) != VK_SUCCESS) return 1;
	if (CreateOffScreenFramebuffer(3, 1, &OffScreenPass[3].ColorImageView[0], VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE) != VK_SUCCESS) return 1;

	return VK_SUCCESS;
}