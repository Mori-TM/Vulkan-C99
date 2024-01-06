VkDescriptorSetAllocateInfo AllocateDescriptorSets(unsigned int Count, VkDescriptorSetLayout* SetLayouts)
{
	VkDescriptorSetAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.descriptorPool = DescriptoPool;
	AllocateInfo.descriptorSetCount = Count;
	AllocateInfo.pSetLayouts = SetLayouts;

	return AllocateInfo;
}

VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet DstSet, unsigned int DstBinding, VkDescriptorType DescriptorType, VkDescriptorImageInfo* ImageInfo, VkDescriptorBufferInfo* BufferInfo)
{
	VkWriteDescriptorSet DescriptorWrite;
	DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorWrite.pNext = NULL;
	DescriptorWrite.dstSet = DstSet;
	DescriptorWrite.dstBinding = DstBinding;
	DescriptorWrite.dstArrayElement = 0;
	DescriptorWrite.descriptorCount = 1;
	DescriptorWrite.descriptorType = DescriptorType;
	DescriptorWrite.pImageInfo = ImageInfo;
	DescriptorWrite.pBufferInfo = BufferInfo;
	DescriptorWrite.pTexelBufferView = NULL;

	return DescriptorWrite;
}

VkResult CreateUniformDescriptorSet(VkDescriptorSet* DescriptorSet, VkDescriptorSetLayout* Layout, VkBuffer UniformBuffer, size_t Size, unsigned int Binding)
{
	VkDescriptorSetAllocateInfo AllocateInfo = AllocateDescriptorSets(1, Layout);
	if (vkAllocateDescriptorSets(Device, &AllocateInfo, DescriptorSet) != VK_SUCCESS)
		return 1;

	VkDescriptorBufferInfo BufferInfo;
	BufferInfo.buffer = UniformBuffer;
	BufferInfo.offset = 0;
	BufferInfo.range = Size;

	VkWriteDescriptorSet DescriptorWrite = WriteDescriptorSet(*DescriptorSet, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NULL, &BufferInfo);

	vkUpdateDescriptorSets(Device, 1, &DescriptorWrite, 0, NULL);

	return VK_SUCCESS;
}

VkResult CreateTextureDescriptorSet(bool Allocate, VkDescriptorSet* DescriptorSet, VkDescriptorSetLayout* Layout, VkSampler Sampler, VkImageView ImageView, VkImageLayout ImageLayout, unsigned int Binding)
{
	if (Allocate)
	{
		VkDescriptorSetAllocateInfo AllocateInfo = AllocateDescriptorSets(1, Layout);

		if (vkAllocateDescriptorSets(Device, &AllocateInfo, DescriptorSet) != VK_SUCCESS)
			return 1;
	}	

	VkDescriptorImageInfo ImageInfo;
	ImageInfo.sampler = Sampler;
	ImageInfo.imageView = ImageView;
	ImageInfo.imageLayout = ImageLayout;

	VkWriteDescriptorSet TextureWriteDescriptorSet = WriteDescriptorSet(*DescriptorSet, Binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &ImageInfo, NULL);

	vkUpdateDescriptorSets(Device, 1, &TextureWriteDescriptorSet, 0, NULL);

	return VK_SUCCESS;
}

VkResult CreateUniformDescriptorSets()
{
	if (CreateUniformDescriptorSet(&UniformDescriptorSet, &UniformDescriptorSetLayout, UniformBuffer, sizeof(UniformBufferObject), 0) != VK_SUCCESS) return 1;

	return VK_SUCCESS;
}

VkResult CreateTextureDescriptorSets()
{
	free(TextureDescriptorSets);
	TextureDescriptorSets = malloc(TextureDescriptorSetsCount * sizeof(VkDescriptorSet));

for (unsigned int i = 0; i < TextureDescriptorSetsCount; i++)
	if (CreateTextureDescriptorSet(true, &TextureDescriptorSets[i], &TextureDescriptorSetLayout, TextureSampler[i], TextureImageViews[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0) != VK_SUCCESS) return 1;

	if (CreateTextureDescriptorSet(true, &OffScreenPass[0].DescriptorSet, &OffScreenPass[0].DescriptorSetLayout, OffScreenPass[0].Sampler, OffScreenPass[0].DepthImageView, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, 0) != VK_SUCCESS) return 1;

	if (CreateTextureDescriptorSet(true, &OffScreenPass[1].DescriptorSet, &OffScreenPass[1].DescriptorSetLayout, OffScreenPass[1].Sampler, OffScreenPass[1].ColorImageView[0], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0) != VK_SUCCESS) return 1;
	if (CreateTextureDescriptorSet(false, &OffScreenPass[1].DescriptorSet, &OffScreenPass[1].DescriptorSetLayout, OffScreenPass[1].Sampler, OffScreenPass[1].ColorImageView[1], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1) != VK_SUCCESS)	return 1;

	if (CreateTextureDescriptorSet(true, &OffScreenPass[2].DescriptorSet, &OffScreenPass[2].DescriptorSetLayout, OffScreenPass[2].Sampler, OffScreenPass[2].ColorImageView[0], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1) != VK_SUCCESS) return 1;

	if (CreateTextureDescriptorSet(true, &OffScreenPass[3].DescriptorSet, &OffScreenPass[3].DescriptorSetLayout, OffScreenPass[3].Sampler, OffScreenPass[3].ColorImageView[0], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0) != VK_SUCCESS) return 1;
	
	return VK_SUCCESS;
}