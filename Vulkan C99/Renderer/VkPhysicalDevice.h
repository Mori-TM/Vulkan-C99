VkResult PickPhysicalDevice()
{
	int DeviceCount = 0;	//Vulkan supported Graphics Card count
	vkEnumeratePhysicalDevices(Instance, &DeviceCount, NULL);

	if (DeviceCount == 0)
		return 1;

	VkPhysicalDevice* PhysicalDevices = malloc(DeviceCount * sizeof(VkPhysicalDevice));
	vkEnumeratePhysicalDevices(Instance, &DeviceCount, PhysicalDevices);

	for (int i = 0; i < DeviceCount; i++)
		PrintStats(PhysicalDevices[i]);

	PhysicalDevice = PhysicalDevices[0];

	VkSampleCountFlagBits Samples = GetMaxSampleCount();
	
	if (MsaaSamples > Samples)
		MsaaSamples = Samples;
	
	printf("Msaa Samples: %d\n", MsaaSamples);

	return VK_SUCCESS;
}