VkResult CreateCommandBuffers()
{
	CommandBuffers = malloc(SwapChainImageViewCount * sizeof(VkCommandBuffer));

	VkCommandBufferAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.commandPool = CommandPool;
	AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocateInfo.commandBufferCount = SwapChainImageViewCount;

	if (vkAllocateCommandBuffers(Device, &AllocateInfo, CommandBuffers) != VK_SUCCESS)
		return 1;

	return VK_SUCCESS;
}