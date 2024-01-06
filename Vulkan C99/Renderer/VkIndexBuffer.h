VkResult CreateIndexBuffer(VkBuffer* IndexBuffer, Index* Indices)
{
	IndexBufferMemory = realloc(IndexBufferMemory, (IndexBufferMemoryCount + 1) * sizeof(VkDeviceMemory));

	VkDeviceSize Size = sizeof(unsigned int) * Indices->Size;

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	if (CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) != VK_SUCCESS)
		return 1;

	void* Data;
	vkMapMemory(Device, StagingBufferMemory, 0, Size, 0, &Data);
	memcpy(Data, Indices->Index, Size);
	vkUnmapMemory(Device, StagingBufferMemory);

	if (CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, &IndexBufferMemory[IndexBufferMemoryCount]) != VK_SUCCESS)
		return 1;

	if (CopyBuffer(StagingBuffer, *IndexBuffer, Size) != VK_SUCCESS)
		return 1;

	vkDestroyBuffer(Device, StagingBuffer, NULL);
	vkFreeMemory(Device, StagingBufferMemory, NULL);
	free(Indices->Index);

	IndexBufferMemoryCount++;

	return VK_SUCCESS;
}