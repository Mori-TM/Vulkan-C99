void UpdateUniformBuffer()
{	
	SandboxUpdate();

	void* Data;
	vkMapMemory(Device, UniformBufferMemory, 0, sizeof(UBO), 0, &Data);
	memcpy(Data, &UBO, sizeof(UBO));
	vkUnmapMemory(Device, UniformBufferMemory);
}