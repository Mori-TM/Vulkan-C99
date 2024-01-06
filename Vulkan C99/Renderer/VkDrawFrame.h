void DrawFrame()
{
	vkWaitForFences(Device, 1, &InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

	vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX, ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &ImageIndex);

	if (InFlightImages[ImageIndex] != VK_NULL_HANDLE)
		vkWaitForFences(Device, 1, &InFlightImages[ImageIndex], VK_TRUE, UINT64_MAX);

	InFlightImages[ImageIndex] = InFlightFences[CurrentFrame];

	VkSemaphore WaitSemaphores[] = { ImageAvailableSemaphores[CurrentFrame] };
	VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore SignalSemaphores[] = { RenderFinishedSemaphores[CurrentFrame] };

	UpdateUniformBuffer();
	RecordCommandBuffer();

	VkSubmitInfo SubmitInfo;
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.waitSemaphoreCount = 1;
	SubmitInfo.pWaitSemaphores = WaitSemaphores;
	SubmitInfo.pWaitDstStageMask = WaitStages;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffers[ImageIndex];
	SubmitInfo.signalSemaphoreCount = 1;
	SubmitInfo.pSignalSemaphores = SignalSemaphores;

	vkResetFences(Device, 1, &InFlightFences[CurrentFrame]);

	StartTimer();
	VkResult Success = vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, InFlightFences[CurrentFrame]);
	TimeToSubmitQueue = EndTimer("");

	if (Success == VK_ERROR_OUT_OF_DATE_KHR)
		RecreateSwapChain();
	else if (Success != VK_SUCCESS)
		printf("Faild to Submit Draw Command Buffer %d\n", Success);

	VkSwapchainKHR SwapChains[] = { SwapChain };

	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pWaitSemaphores = SignalSemaphores;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = SwapChains;
	PresentInfo.pImageIndices = &ImageIndex;
	PresentInfo.pResults = NULL;

	StartTimer();
	Success = vkQueuePresentKHR(PresentQueue, &PresentInfo);
	TimeToPresentQueue = EndTimer("");

	if (Success == VK_ERROR_OUT_OF_DATE_KHR || Success == VK_SUBOPTIMAL_KHR)
		RecreateSwapChain();
	else if (Success != VK_SUCCESS)
		printf("Failed to submit Draw Command Buffer\n");

//	PrintRenderTimes();

	CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}