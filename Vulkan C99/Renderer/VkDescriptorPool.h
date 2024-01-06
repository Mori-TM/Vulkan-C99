VkResult CreateDescriptorPool()
{
	VkDescriptorPoolSize PoolSize[2];
	PoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	PoolSize[0].descriptorCount = 1;

	PoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	PoolSize[1].descriptorCount = 11;

	VkDescriptorPoolCreateInfo PoolInfo;
	PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	PoolInfo.pNext = NULL;
	PoolInfo.flags = 0;
	PoolInfo.maxSets = 8192;	//maximum number of descriptor sets that may be allocated
	PoolInfo.poolSizeCount = 2;
	PoolInfo.pPoolSizes = PoolSize;

	return vkCreateDescriptorPool(Device, &PoolInfo, NULL, &DescriptoPool);
}