VkResult CreateVertexBuffer(VkBuffer* VertexBuffer, Vertex* Vertices)
{
	VertexBufferMemory = realloc(VertexBufferMemory, (VertexBufferMemoryCount + 1) * sizeof(VkDeviceMemory));

	VkDeviceSize Size = sizeof(VertexCore) * Vertices->Size;

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;
	if (CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) != VK_SUCCESS)
		return 1;

	void* Data;
	vkMapMemory(Device, StagingBufferMemory, 0, Size, 0, &Data);
	memcpy(Data, Vertices->Vertex, Size);
	vkUnmapMemory(Device, StagingBufferMemory);

	if (CreateBuffer(Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, &VertexBufferMemory[VertexBufferMemoryCount]) != VK_SUCCESS)
		return 1;

	if (CopyBuffer(StagingBuffer, *VertexBuffer, Size) != VK_SUCCESS)
		return 1;

	vkDestroyBuffer(Device, StagingBuffer, NULL);
	vkFreeMemory(Device, StagingBufferMemory, NULL);
	free(Vertices->Vertex);

	VertexBufferMemoryCount++;

	return VK_SUCCESS;
}