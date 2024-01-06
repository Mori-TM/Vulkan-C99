VkAttachmentDescription CreateAttachmentDescription()
{
	VkAttachmentDescription AttachmentDescription;
	AttachmentDescription.flags = 0;
	AttachmentDescription.format = SwapChainImageFormat;
	AttachmentDescription.samples = MsaaSamples;
	AttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	AttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	AttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	AttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	AttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	AttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	return AttachmentDescription;
}

VkAttachmentReference CreateAttachmentReference(unsigned int Attachment, VkImageLayout Layout)
{
	VkAttachmentReference AttachmentReference;
	AttachmentReference.attachment = Attachment;
	AttachmentReference.layout = Layout;

	return AttachmentReference;
}

VkSubpassDescription CreateSubpassDescription()
{
	VkSubpassDescription Subpass;
	Subpass.flags = 0;
	Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	Subpass.inputAttachmentCount = 0;
	Subpass.pInputAttachments = NULL;
	Subpass.colorAttachmentCount = 0;
	Subpass.pColorAttachments = NULL;
	Subpass.pResolveAttachments = NULL;
	Subpass.pDepthStencilAttachment = NULL;
	Subpass.preserveAttachmentCount = NULL;
	Subpass.pPreserveAttachments = NULL;

	return Subpass;
}

VkRenderPassCreateInfo CreateRenderPassCreateInfo()
{
	VkRenderPassCreateInfo RenderPassInfo;
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.flags = 0;
	RenderPassInfo.attachmentCount = 0;
	RenderPassInfo.pAttachments = NULL;
	RenderPassInfo.subpassCount = 0;
	RenderPassInfo.pSubpasses = NULL;
	RenderPassInfo.dependencyCount = 0;
	RenderPassInfo.pDependencies = NULL;

	return RenderPassInfo;
}

VkResult CreateSwapChainRenderPass()
{
	VkAttachmentDescription ColorAttachment = CreateAttachmentDescription();
	ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription DepthAttachment = CreateAttachmentDescription();
	DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	DepthAttachment.format = FindDepthFormat();
	DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkAttachmentReference ColorAttachmentReference = CreateAttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkAttachmentReference DepthAttachmentReference = CreateAttachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	VkSubpassDescription Subpass = CreateSubpassDescription();
	Subpass.colorAttachmentCount = 1;
	Subpass.pColorAttachments = &ColorAttachmentReference;
	Subpass.pResolveAttachments = NULL;
	Subpass.pDepthStencilAttachment = &DepthAttachmentReference;

	VkSubpassDependency Dependency;
	Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	Dependency.dstSubpass = 0;
	Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	Dependency.srcAccessMask = 0;
	Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	Dependency.dependencyFlags = 0;

	VkAttachmentDescription Attachments[] = { ColorAttachment, DepthAttachment };

	VkRenderPassCreateInfo RenderPassInfo = CreateRenderPassCreateInfo();
	RenderPassInfo.attachmentCount = 2;
	RenderPassInfo.pAttachments = Attachments;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &Subpass;
	RenderPassInfo.dependencyCount = 1;
	RenderPassInfo.pDependencies = &Dependency;

	VkResult Success = vkCreateRenderPass(Device, &RenderPassInfo, NULL, &SwapChainRenderPass);
	return Success;
}

VkResult CreateOffScreenRenderPass(unsigned int Index, unsigned int ColorAttachmentCount, bool HasDepthAttachment, bool Msaa)
{
	VkAttachmentDescription ColorAttachment;
	VkAttachmentDescription DepthAttachment;
	VkAttachmentDescription ColorAttachmentResolve;

	if (ColorAttachmentCount > 0)
	{
		ColorAttachment = CreateAttachmentDescription();
		ColorAttachment.samples = Msaa ? MsaaSamples : VK_SAMPLE_COUNT_1_BIT;
		ColorAttachment.finalLayout = Msaa ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}	

	if (HasDepthAttachment && ColorAttachmentCount > 0)
	{
		DepthAttachment = CreateAttachmentDescription();
		DepthAttachment = CreateAttachmentDescription();
		DepthAttachment.format = FindDepthFormat();
		DepthAttachment.samples = Msaa ? MsaaSamples : VK_SAMPLE_COUNT_1_BIT;
		DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	else if (HasDepthAttachment && ColorAttachmentCount == 0)
	{
		DepthAttachment = CreateAttachmentDescription();
		DepthAttachment.format = FindDepthFormat();
		DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
	}
	
	if (Msaa)
	{
		ColorAttachmentResolve.flags = 0;
		ColorAttachmentResolve.format = SwapChainImageFormat;
		ColorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
	

	VkAttachmentReference* ColorAttachmentReferences = malloc(ColorAttachmentCount * sizeof(VkAttachmentReference));
	VkAttachmentReference  DepthAttachmentReference;
	VkAttachmentReference* ColorAttachmentResolveReferences = NULL;

	for (unsigned int i = 0; i < ColorAttachmentCount; i++)
		ColorAttachmentReferences[i] = CreateAttachmentReference(i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	
	if (HasDepthAttachment)
		DepthAttachmentReference = CreateAttachmentReference(ColorAttachmentCount, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	if (Msaa)
	{
		unsigned int ResolveRefrenceCount = 0;
		ColorAttachmentResolveReferences = malloc(ColorAttachmentCount * sizeof(VkAttachmentReference));;
		for (unsigned int i = ColorAttachmentCount + HasDepthAttachment; i < (ColorAttachmentCount * 2) + HasDepthAttachment; i++)
		{
			ColorAttachmentResolveReferences[ResolveRefrenceCount].attachment = i;
			ColorAttachmentResolveReferences[ResolveRefrenceCount].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			ResolveRefrenceCount++;
		}
	}
	
	VkSubpassDescription Subpass = CreateSubpassDescription();
	Subpass.colorAttachmentCount = ColorAttachmentCount;
	Subpass.pColorAttachments = ColorAttachmentReferences;
	Subpass.pResolveAttachments = Msaa ? ColorAttachmentResolveReferences : NULL;
	Subpass.pDepthStencilAttachment = HasDepthAttachment ? &DepthAttachmentReference : NULL;

	VkSubpassDependency Dependencies[2];
	Dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	Dependencies[0].dstSubpass = 0;
	Dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	Dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	Dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	Dependencies[1].srcSubpass = 0;
	Dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	Dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	Dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	Dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	if (ColorAttachmentCount > 0)
	{
		Dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		Dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}
	else
	{
		Dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		Dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		Dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		Dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}
	
	unsigned int AttachmentCount = (ColorAttachmentCount * (Msaa ? 2 : 1) + HasDepthAttachment);
	VkAttachmentDescription* Attachments = malloc(AttachmentCount * sizeof(VkAttachmentDescription));
	for (unsigned int i = 0; i < ColorAttachmentCount; i++)
		Attachments[i] = ColorAttachment;
	for (unsigned int i = ColorAttachmentCount; i < ColorAttachmentCount + HasDepthAttachment; i++)
		Attachments[i] = DepthAttachment;
	for (unsigned int i = ColorAttachmentCount + HasDepthAttachment; i < AttachmentCount; i++)
		Attachments[i] = ColorAttachmentResolve;

	
	//	= { ColorAttachment, ColorAttachment, DepthAttachment, ColorAttachmentResolve, ColorAttachmentResolve };

	VkRenderPassCreateInfo RenderPassInfo = CreateRenderPassCreateInfo();
	RenderPassInfo = CreateRenderPassCreateInfo();
	RenderPassInfo.attachmentCount = AttachmentCount;
	RenderPassInfo.pAttachments = Attachments;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &Subpass;
	RenderPassInfo.dependencyCount = 2;
	RenderPassInfo.pDependencies = Dependencies;

	if (vkCreateRenderPass(Device, &RenderPassInfo, NULL, &OffScreenPass[Index].RenderPass) != VK_SUCCESS)
		return 1;

	return VK_SUCCESS;
}

VkResult CreateOffScreenRenderPasses()
{
	CreateOffScreenRenderPass(0, 0, true, false);

	CreateOffScreenRenderPass(1, 2, true, true);

	CreateOffScreenRenderPass(2, 1, false, false);
	CreateOffScreenRenderPass(3, 1, false, false);

	return VK_SUCCESS;
}