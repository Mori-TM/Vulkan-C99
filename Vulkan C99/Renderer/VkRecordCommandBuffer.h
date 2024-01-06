VkResult RecordCommandBuffer()
{
	VkCommandBufferBeginInfo BeginInfo;
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	BeginInfo.pNext = NULL;
	BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	BeginInfo.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(CommandBuffers[ImageIndex], &BeginInfo) != VK_SUCCESS)
		return 1;

	VkRenderPassBeginInfo RenderPassBeginInfo;
	VkClearValue ClearColorValue[2] = { {ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w}, {ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w} };
	VkClearValue ClearDepthStencilValue = { 1.0, 0.0 };

	StartTimer();
	//Render Scene Depth from Light Perspective
	{
		
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = OffScreenPass[0].RenderPass;
		RenderPassBeginInfo.framebuffer = OffScreenPass[0].Framebuffers[ImageIndex];
		RenderPassBeginInfo.renderArea.offset.x = 0;
		RenderPassBeginInfo.renderArea.offset.y = 0;
		RenderPassBeginInfo.renderArea.extent.width = OffScreenPass[0].Width;
		RenderPassBeginInfo.renderArea.extent.height = OffScreenPass[0].Height;
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearDepthStencilValue;

		vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		SandboxRenderDepth();

		vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
	}
	TimeToRenderDepth = EndTimer("Time To Render Depth: ");
	
	StartTimer();
	//Render Scene Normaly to two Textures
	{
		VkClearValue ClearValues[] = { ClearColorValue[0], ClearColorValue[1], ClearDepthStencilValue };

		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = OffScreenPass[1].RenderPass;
		RenderPassBeginInfo.framebuffer = OffScreenPass[1].Framebuffers[ImageIndex];
		RenderPassBeginInfo.renderArea.offset.x = 0;
		RenderPassBeginInfo.renderArea.offset.y = 0;
		RenderPassBeginInfo.renderArea.extent.width = OffScreenPass[1].Width;
		RenderPassBeginInfo.renderArea.extent.height = OffScreenPass[1].Height;
		RenderPassBeginInfo.clearValueCount = 3;
		RenderPassBeginInfo.pClearValues = ClearValues;

		vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		SandboxRender();

		vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
	}
	TimeToRenderScene = EndTimer("Time To Render Scene: ");

	StartTimer();
	//Horizontal Blur
	{
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = OffScreenPass[2].RenderPass;
		RenderPassBeginInfo.framebuffer = OffScreenPass[2].Framebuffers[ImageIndex];
		RenderPassBeginInfo.renderArea.offset.x = 0;
		RenderPassBeginInfo.renderArea.offset.y = 0;
		RenderPassBeginInfo.renderArea.extent.width = OffScreenPass[2].Width;
		RenderPassBeginInfo.renderArea.extent.height = OffScreenPass[2].Height;
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearColorValue[1];

		vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindPipeline(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, OffScreenPass[2].GraphicsPipeline);
		BlurFragmentConst.Horizontal = true;
		vkCmdPushConstants(CommandBuffers[ImageIndex], OffScreenPass[2].PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(BlurFragmentPushConstant), &BlurFragmentConst);
		vkCmdBindVertexBuffers(CommandBuffers[ImageIndex], 0, 1, &QuadVertexBuffer3D, Offsets);
		vkCmdBindIndexBuffer(CommandBuffers[ImageIndex], QuadIndexBuffer3D, 0, VK_INDEX_TYPE_UINT32);
	
		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, OffScreenPass[2].PipelineLayout, 0, 1, &OffScreenPass[1].DescriptorSet, 0, NULL);
		vkCmdDrawIndexed(CommandBuffers[ImageIndex], QuadIndices3D.Size, 1, 0, 0, 0);

		vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
	}

	//Vertical Blur
	{
		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = OffScreenPass[3].RenderPass;
		RenderPassBeginInfo.framebuffer = OffScreenPass[3].Framebuffers[ImageIndex];
		RenderPassBeginInfo.renderArea.offset.x = 0;
		RenderPassBeginInfo.renderArea.offset.y = 0;
		RenderPassBeginInfo.renderArea.extent.width = OffScreenPass[3].Width;
		RenderPassBeginInfo.renderArea.extent.height = OffScreenPass[3].Height;
		RenderPassBeginInfo.clearValueCount = 1;
		RenderPassBeginInfo.pClearValues = &ClearColorValue[1];

		vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindPipeline(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, OffScreenPass[3].GraphicsPipeline);
		BlurFragmentConst.Horizontal = false;
		vkCmdPushConstants(CommandBuffers[ImageIndex], OffScreenPass[3].PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(BlurFragmentPushConstant), &BlurFragmentConst);
		vkCmdBindVertexBuffers(CommandBuffers[ImageIndex], 0, 1, &QuadVertexBuffer3D, Offsets);
		vkCmdBindIndexBuffer(CommandBuffers[ImageIndex], QuadIndexBuffer3D, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(CommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, OffScreenPass[3].PipelineLayout, 0, 1, &OffScreenPass[2].DescriptorSet, 0, NULL);
		vkCmdDrawIndexed(CommandBuffers[ImageIndex], QuadIndices3D.Size, 1, 0, 0, 0);

		vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
	}
	TimeToRenderBlur = EndTimer("Time To Blur Scene: ");
	
	StartTimer();
	//Render GUI
	{
		VkClearValue ClearValues[] = { ClearColorValue[0], ClearDepthStencilValue };

		RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassBeginInfo.pNext = NULL;
		RenderPassBeginInfo.renderPass = SwapChainRenderPass;
		RenderPassBeginInfo.framebuffer = SwapChainFramebuffers[ImageIndex];
		RenderPassBeginInfo.renderArea.offset.x = 0;
		RenderPassBeginInfo.renderArea.offset.y = 0;
		RenderPassBeginInfo.renderArea.extent = SwapChainExtent;
		RenderPassBeginInfo.clearValueCount = 2;
		RenderPassBeginInfo.pClearValues = ClearValues;

		vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		SandboxRenderGUI();
		
		vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
	}
	TimeToRenderGUI = EndTimer("Time To Render GUI: ");

	if (vkEndCommandBuffer(CommandBuffers[ImageIndex]) != VK_SUCCESS)
		return 1;

	return VK_SUCCESS;
}