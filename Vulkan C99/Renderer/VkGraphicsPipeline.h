VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderModule ShaderModule, VkShaderStageFlagBits ShaderType)
{
	VkPipelineShaderStageCreateInfo ShaderStageInfo;
	ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStageInfo.pNext = NULL;
	ShaderStageInfo.flags = 0;
	ShaderStageInfo.stage = ShaderType;
	ShaderStageInfo.module = ShaderModule;
	ShaderStageInfo.pName = "main";
	ShaderStageInfo.pSpecializationInfo = NULL;

	return ShaderStageInfo;
}

VkVertexInputBindingDescription BindingDescription;
VkVertexInputAttributeDescription AttributeDescriptions[4];

VkPipelineVertexInputStateCreateInfo CreateVertexInputState()
{	
	BindingDescription.binding = 0;
	BindingDescription.stride = sizeof(VertexCore);
	BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
	AttributeDescriptions[0].location = 0;
	AttributeDescriptions[0].binding = 0;
	AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	AttributeDescriptions[0].offset = 0;

	AttributeDescriptions[1].location = 1;
	AttributeDescriptions[1].binding = 0;
	AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	AttributeDescriptions[1].offset = 12;

	AttributeDescriptions[2].location = 2;
	AttributeDescriptions[2].binding = 0;
	AttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	AttributeDescriptions[2].offset = 24;

	AttributeDescriptions[3].location = 3;
	AttributeDescriptions[3].binding = 0;
	AttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
	AttributeDescriptions[3].offset = 32;

	VkPipelineVertexInputStateCreateInfo VertexInputInfo;
	VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputInfo.pNext = NULL;
	VertexInputInfo.flags = 0;
	VertexInputInfo.vertexBindingDescriptionCount = 1;
	VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
	VertexInputInfo.vertexAttributeDescriptionCount = 4;
	VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions;

	return VertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyState(VkPrimitiveTopology Topology)
{
	VkPipelineInputAssemblyStateCreateInfo InputAssembly;
	InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssembly.pNext = NULL;
	InputAssembly.flags = 0;
	InputAssembly.topology = Topology;
	InputAssembly.primitiveRestartEnable = VK_FALSE;

	return InputAssembly;
}

VkViewport Viewport;
VkRect2D Scissor;

VkPipelineViewportStateCreateInfo CreateViewportStateInfo(float x, float y, float Width, float Height)
{
	Viewport.x = x;
	Viewport.y = y;
	Viewport.width = Width;
	Viewport.height = Height;
	Viewport.minDepth = 0.0;
	Viewport.maxDepth = 1.0;

	VkOffset2D Offset;
	Offset.x = 0;
	Offset.y = 0;

	Scissor.offset = Offset;
	Scissor.extent.width = Width;
	Scissor.extent.height = Height;

	VkPipelineViewportStateCreateInfo ViewportState;
	ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportState.pNext = NULL;
	ViewportState.flags = 0;
	ViewportState.viewportCount = 1;
	ViewportState.pViewports = &Viewport;
	ViewportState.scissorCount = 1;
	ViewportState.pScissors = &Scissor;

	return ViewportState;
}

VkPipelineRasterizationStateCreateInfo CreateRasterizerStateInfo(VkCullModeFlags CullMode, VkFrontFace FrontFace, VkPolygonMode PolygonMode, VkBool32 DepthBiasEnable, float LineWidth)
{
	VkPipelineRasterizationStateCreateInfo Rasterizer;
	Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	Rasterizer.pNext = NULL;
	Rasterizer.flags = 0;
	Rasterizer.depthClampEnable = VK_FALSE;
	Rasterizer.rasterizerDiscardEnable = VK_FALSE;
	Rasterizer.polygonMode = PolygonMode;
	Rasterizer.lineWidth = LineWidth;
	Rasterizer.cullMode = CullMode;
	Rasterizer.frontFace = FrontFace;
	Rasterizer.depthBiasEnable = DepthBiasEnable;
	Rasterizer.depthBiasConstantFactor = 1.25;
	Rasterizer.depthBiasClamp = 0.0;
	Rasterizer.depthBiasSlopeFactor = 1.75;

	return Rasterizer;
}

VkPipelineMultisampleStateCreateInfo CreateMultiSampleStateInfo(VkBool32 SampleShading, VkSampleCountFlagBits Samples)
{
	VkPipelineMultisampleStateCreateInfo Multisampling;
	Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	Multisampling.pNext = NULL;
	Multisampling.flags = 0;
	Multisampling.sampleShadingEnable = SampleShading;
	Multisampling.rasterizationSamples = Samples;
	Multisampling.minSampleShading = 0.2;
	Multisampling.pSampleMask = NULL;
	Multisampling.alphaToCoverageEnable = VK_FALSE;
	Multisampling.alphaToOneEnable = VK_FALSE;

	return Multisampling;
}

VkPipelineDepthStencilStateCreateInfo CreateDepthStencilStateInfo(VkBool32 DepthTest, VkCompareOp CompareOp)
{
	VkPipelineDepthStencilStateCreateInfo DepthStencil;
	DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DepthStencil.pNext = NULL;
	DepthStencil.flags = 0;
	DepthStencil.depthTestEnable = DepthTest;
	DepthStencil.depthWriteEnable = VK_TRUE;
	DepthStencil.depthCompareOp = CompareOp;
	DepthStencil.depthBoundsTestEnable = VK_FALSE;
	DepthStencil.stencilTestEnable = VK_FALSE;
	DepthStencil.front.failOp = VK_NULL_HANDLE;
	DepthStencil.front.passOp = VK_NULL_HANDLE;
	DepthStencil.front.depthFailOp = VK_NULL_HANDLE;
	DepthStencil.front.compareOp = VK_NULL_HANDLE;
	DepthStencil.front.compareMask = VK_NULL_HANDLE;
	DepthStencil.front.writeMask = VK_NULL_HANDLE;
	DepthStencil.front.reference = VK_NULL_HANDLE;
	DepthStencil.back.failOp = VK_NULL_HANDLE;
	DepthStencil.back.passOp = VK_NULL_HANDLE;
	DepthStencil.back.depthFailOp = VK_NULL_HANDLE;
	DepthStencil.back.compareOp = VK_NULL_HANDLE;
	DepthStencil.back.compareMask = VK_NULL_HANDLE;
	DepthStencil.back.writeMask = VK_NULL_HANDLE;
	DepthStencil.back.reference = VK_NULL_HANDLE;
	DepthStencil.minDepthBounds = 0.0;
	DepthStencil.maxDepthBounds = 1.0;

	return DepthStencil;
}

VkPipelineColorBlendAttachmentState* ColorBlendAttachments;

VkPipelineColorBlendStateCreateInfo CreateColorBlendState(unsigned int BelendAttachmentCount, VkBool32 AlphaBlend)
{
	ColorBlendAttachments = malloc(BelendAttachmentCount * sizeof(VkPipelineColorBlendAttachmentState));

	for (unsigned int i = 0; i < BelendAttachmentCount; i++)
	{
		ColorBlendAttachments[i].blendEnable = AlphaBlend;
		ColorBlendAttachments[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		ColorBlendAttachments[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		ColorBlendAttachments[i].colorBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachments[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachments[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	}
	
	VkPipelineColorBlendStateCreateInfo ColorBlending;
	ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlending.pNext = NULL;
	ColorBlending.flags = 0;
	ColorBlending.logicOpEnable = VK_FALSE;
	ColorBlending.logicOp = VK_LOGIC_OP_COPY;
	ColorBlending.attachmentCount = BelendAttachmentCount;
	ColorBlending.pAttachments = ColorBlendAttachments;
	ColorBlending.blendConstants[0] = 0.0;
	ColorBlending.blendConstants[1] = 0.0;
	ColorBlending.blendConstants[2] = 0.0;
	ColorBlending.blendConstants[3] = 0.0;

	return ColorBlending;
}

VkResult CreatePipelineLayout(VkPipelineLayout* Layout, unsigned int LayoutCount, VkDescriptorSetLayout Layouts[], unsigned int PushConstantCount, VkPushConstantRange PushConstants[])
{
	VkPipelineLayoutCreateInfo PipeLineLayoutInfo;
	PipeLineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipeLineLayoutInfo.pNext = NULL;
	PipeLineLayoutInfo.flags = 0;
	PipeLineLayoutInfo.setLayoutCount = LayoutCount;
	PipeLineLayoutInfo.pSetLayouts = Layouts;
	PipeLineLayoutInfo.pushConstantRangeCount = PushConstantCount;
	PipeLineLayoutInfo.pPushConstantRanges = PushConstants;

	return vkCreatePipelineLayout(Device, &PipeLineLayoutInfo, NULL, Layout);
}

VkResult CreateOffscreenGraphicsPipeline(int Index, const char* VertexPath, const char* FragmentPath, bool Msaa, unsigned int BlendAttachmentCount, size_t VertexPushConstantSize, size_t FragmentPushConstantSize, unsigned int Offset, unsigned int DescriptorSetLayoutCount, VkDescriptorSetLayout* DescriptorSetLayouts)
{
	size_t VertexShaderLength;
	size_t FragmentShaderLength;
	char* VertexShader = ReadFileData(VertexPath, &VertexShaderLength);
	char* FragmentShader = ReadFileData(FragmentPath, &FragmentShaderLength);

	VkShaderModule VertexShaderModule = CreateShaderModule(VertexShader, VertexShaderLength);
	VkShaderModule FragmentShaderModule = CreateShaderModule(FragmentShader, FragmentShaderLength);

	VkPipelineShaderStageCreateInfo VertexShaderStageInfo = CreateShaderStageInfo(VertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
	VkPipelineShaderStageCreateInfo FragmentShaderStageInfo = CreateShaderStageInfo(FragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo ShaderStages[] = { VertexShaderStageInfo, FragmentShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo VertexInputInfo = CreateVertexInputState();

	VkPipelineInputAssemblyStateCreateInfo InputAssembly = CreateInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	VkPipelineViewportStateCreateInfo ViewportState = CreateViewportStateInfo(0.0, 0.0, OffScreenPass[Index].Width, OffScreenPass[Index].Height);

	VkPipelineRasterizationStateCreateInfo Rasterizer = CreateRasterizerStateInfo(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_POLYGON_MODE_FILL, VK_FALSE, 1.0);

	VkPipelineMultisampleStateCreateInfo Multisampling = CreateMultiSampleStateInfo(VK_TRUE, Msaa ? MsaaSamples : VK_SAMPLE_COUNT_1_BIT);

	VkPipelineDepthStencilStateCreateInfo DepthStencil = CreateDepthStencilStateInfo(VK_TRUE, VK_COMPARE_OP_LESS);

	VkPipelineColorBlendStateCreateInfo ColorBlending = CreateColorBlendState(BlendAttachmentCount, VK_TRUE);

	int PushCount = 0;
	VkPushConstantRange PushConstantRange[2];
	if (VertexPushConstantSize > 0)
	{
		PushConstantRange[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		PushConstantRange[0].offset = 0;
		PushConstantRange[0].size = VertexPushConstantSize;
		PushCount = 1;
	}
	if (FragmentPushConstantSize > 0 && VertexPushConstantSize == 0)
	{
		PushConstantRange[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		PushConstantRange[0].offset = 0;
		PushConstantRange[0].size = FragmentPushConstantSize;
		PushCount = 1;
	}
	else if (FragmentPushConstantSize > 0 && VertexPushConstantSize > 0)
	{
		PushConstantRange[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		PushConstantRange[1].offset = Offset;
		PushConstantRange[1].size = FragmentPushConstantSize;
		PushCount = 2;
	}
	

	CreatePipelineLayout(&OffScreenPass[Index].PipelineLayout, DescriptorSetLayoutCount, DescriptorSetLayouts, PushCount, PushConstantRange);

	VkGraphicsPipelineCreateInfo PipelineInfo;
	PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineInfo.pNext = NULL;
	PipelineInfo.flags = 0;
	PipelineInfo.stageCount = 2;
	PipelineInfo.pStages = ShaderStages;
	PipelineInfo.pVertexInputState = &VertexInputInfo;
	PipelineInfo.pInputAssemblyState = &InputAssembly;
	PipelineInfo.pTessellationState = NULL;
	PipelineInfo.pViewportState = &ViewportState;
	PipelineInfo.pRasterizationState = &Rasterizer;
	PipelineInfo.pMultisampleState = &Multisampling;
	PipelineInfo.pDepthStencilState = &DepthStencil;
	PipelineInfo.pColorBlendState = &ColorBlending;
	PipelineInfo.pDynamicState = NULL;
	PipelineInfo.layout = OffScreenPass[Index].PipelineLayout;
	PipelineInfo.renderPass = OffScreenPass[Index].RenderPass;
	PipelineInfo.subpass = 0;
	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &OffScreenPass[Index].GraphicsPipeline) != VK_SUCCESS)
		return 1;

	vkDestroyShaderModule(Device, VertexShaderModule, NULL);
	vkDestroyShaderModule(Device, FragmentShaderModule, NULL);

	return VK_SUCCESS;
}

VkResult CreateGUIGraphicsPipeline()
{
	size_t VertexShaderLength;
	size_t FragmentShaderLength;
	char* VertexShader = ReadFileData("Shader/GUIVertex.spv", &VertexShaderLength);
	char* FragmentShader = ReadFileData("Shader/GUIFragment.spv", &FragmentShaderLength);

	VkShaderModule VertexShaderModule = CreateShaderModule(VertexShader, VertexShaderLength);
	VkShaderModule FragmentShaderModule = CreateShaderModule(FragmentShader, FragmentShaderLength);

	VkPipelineShaderStageCreateInfo VertexShaderStageInfo = CreateShaderStageInfo(VertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
	VkPipelineShaderStageCreateInfo FragmentShaderStageInfo = CreateShaderStageInfo(FragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineShaderStageCreateInfo ShaderStages[] = { VertexShaderStageInfo, FragmentShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo VertexInputInfo = CreateVertexInputState();

	VkPipelineInputAssemblyStateCreateInfo InputAssembly = CreateInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	VkPipelineViewportStateCreateInfo ViewportState = CreateViewportStateInfo(0.0, 0.0, SwapChainExtent.width, SwapChainExtent.height);

	VkPipelineRasterizationStateCreateInfo Rasterizer = CreateRasterizerStateInfo(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_POLYGON_MODE_FILL, VK_FALSE, 2.0);

	VkPipelineMultisampleStateCreateInfo Multisampling = CreateMultiSampleStateInfo(VK_TRUE, VK_SAMPLE_COUNT_1_BIT);

	VkPipelineDepthStencilStateCreateInfo DepthStencil = CreateDepthStencilStateInfo(VK_TRUE, VK_COMPARE_OP_LESS);

	VkPipelineColorBlendStateCreateInfo ColorBlending = CreateColorBlendState(1, VK_TRUE);

	VkPushConstantRange PushConstantRange[2];
	PushConstantRange[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	PushConstantRange[0].offset = VERTEX_OFFSET;
	PushConstantRange[0].size = sizeof(VertexPushConstant);

	PushConstantRange[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	PushConstantRange[1].offset = FRAGMENT_OFFSET;
	PushConstantRange[1].size = sizeof(GUIFragmentPushConstant);

	VkDescriptorSetLayout SetLayouts[] = 
	{ 
		TextureDescriptorSetLayout, 
		OffScreenPass[0].DescriptorSetLayout, 
		OffScreenPass[1].DescriptorSetLayout, 
		OffScreenPass[3].DescriptorSetLayout 
	};

	CreatePipelineLayout(&GUIPipelineLayout, ARRAYSIZE(SetLayouts), SetLayouts, 2, PushConstantRange);

	VkGraphicsPipelineCreateInfo PipelineInfo;
	PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineInfo.pNext = NULL;
	PipelineInfo.flags = 0;
	PipelineInfo.stageCount = 2;
	PipelineInfo.pStages = ShaderStages;
	PipelineInfo.pVertexInputState = &VertexInputInfo;
	PipelineInfo.pInputAssemblyState = &InputAssembly;
	PipelineInfo.pTessellationState = NULL;
	PipelineInfo.pViewportState = &ViewportState;
	PipelineInfo.pRasterizationState = &Rasterizer;
	PipelineInfo.pMultisampleState = &Multisampling;
	PipelineInfo.pDepthStencilState = &DepthStencil;
	PipelineInfo.pColorBlendState = &ColorBlending;
	PipelineInfo.pDynamicState = NULL;
	PipelineInfo.layout = GUIPipelineLayout;
	PipelineInfo.renderPass = SwapChainRenderPass;
	PipelineInfo.subpass = 0;
	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &PipelineInfo, NULL, &GUIGraphicsPipeline) != VK_SUCCESS)
		return 1;

	vkDestroyShaderModule(Device, VertexShaderModule, NULL);
	vkDestroyShaderModule(Device, FragmentShaderModule, NULL);

	return VK_SUCCESS;
}

VkResult CreateOffScreenGraphicsPipelines()
{
	VkDescriptorSetLayout SetLayouts[] =
	{
		UniformDescriptorSetLayout,
		TextureDescriptorSetLayout,
		OffScreenPass[0].DescriptorSetLayout,
	};

	if (CreateOffscreenGraphicsPipeline(0, "Shader/OffScreenVertex.spv", "Shader/OffScreenFragment.spv", VK_FALSE, 1, sizeof(VertexPushConstant), 0, 0, 0, NULL) != VK_SUCCESS) return 1;
	if (CreateOffscreenGraphicsPipeline(1, "Shader/Vertex.spv", "Shader/Fragment.spv", VK_TRUE, 2, sizeof(VertexPushConstant), sizeof(FragmentPushConstant), FRAGMENT_OFFSET, 3, SetLayouts) != VK_SUCCESS) return 1;

	if (CreateOffscreenGraphicsPipeline(2, "Shader/BlurVertex.spv", "Shader/BlurFragment.spv", VK_FALSE, 1, 0, sizeof(BlurFragmentPushConstant), 0, 1, &OffScreenPass[1].DescriptorSetLayout) != VK_SUCCESS) return 1;
	if (CreateOffscreenGraphicsPipeline(3, "Shader/BlurVertex.spv", "Shader/BlurFragment.spv", VK_FALSE, 1, 0, sizeof(BlurFragmentPushConstant), 0, 1, &OffScreenPass[2].DescriptorSetLayout) != VK_SUCCESS) return 1;

	free(ColorBlendAttachments);

	return VK_SUCCESS;
}