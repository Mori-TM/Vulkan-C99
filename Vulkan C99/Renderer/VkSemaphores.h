VkResult CreateSemaphores()
{
	InFlightImages = malloc(SwapChainImageViewCount * sizeof(VkFence));

	for (unsigned int i = 0; i < SwapChainImageViewCount; i++)
		InFlightImages[i] = VK_NULL_HANDLE;

	VkSemaphoreCreateInfo SemaphoreInfo;
	SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreInfo.pNext = NULL;
	SemaphoreInfo.flags = 0;

	VkFenceCreateInfo FenceInfo;
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.pNext = NULL;
	FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		if (vkCreateSemaphore(Device, &SemaphoreInfo, NULL, &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(Device, &SemaphoreInfo, NULL, &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(Device, &FenceInfo, NULL, &InFlightFences[i]) != VK_SUCCESS)
			return 1;

	return VK_SUCCESS;
}