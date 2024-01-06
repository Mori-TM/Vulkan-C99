VkResult CreateCommandPool()
{
	QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);

	VkCommandPoolCreateInfo PoolInfo;
	PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	PoolInfo.pNext = NULL;
	PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	PoolInfo.queueFamilyIndex = Indices.GraphicsFamily;

	return vkCreateCommandPool(Device, &PoolInfo, NULL, &CommandPool);
}