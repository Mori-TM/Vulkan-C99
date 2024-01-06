VkResult CreateLogicalDevice()
{
	float QueuePriority = 1.0;
	QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);

	VkDeviceQueueCreateInfo DeviceQueueCreateInfos[2];
	DeviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueCreateInfos[0].pNext = NULL;
	DeviceQueueCreateInfos[0].flags = 0;
	DeviceQueueCreateInfos[0].queueFamilyIndex = Indices.GraphicsFamily;
	DeviceQueueCreateInfos[0].queueCount = 1;
	DeviceQueueCreateInfos[0].pQueuePriorities = &QueuePriority;
	
	DeviceQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	DeviceQueueCreateInfos[1].pNext = NULL;
	DeviceQueueCreateInfos[1].flags = 0;
	DeviceQueueCreateInfos[1].queueFamilyIndex = Indices.PresentFamily;
	DeviceQueueCreateInfos[1].queueCount = 1;
	DeviceQueueCreateInfos[1].pQueuePriorities = &QueuePriority;

	VkPhysicalDeviceFeatures DeviceFeatures = { VK_FALSE };
	DeviceFeatures.samplerAnisotropy = VK_TRUE;
	DeviceFeatures.sampleRateShading = VK_TRUE;
	DeviceFeatures.fillModeNonSolid = VK_TRUE;
	DeviceFeatures.wideLines = VK_TRUE;

	VkDeviceCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.queueCreateInfoCount = 2;
	CreateInfo.pQueueCreateInfos = DeviceQueueCreateInfos;
	CreateInfo.pEnabledFeatures = &DeviceFeatures;
	CreateInfo.enabledLayerCount = ValidationLayerCount;
	CreateInfo.ppEnabledLayerNames = ValidationLayers;
	CreateInfo.enabledExtensionCount = 1;
	CreateInfo.ppEnabledExtensionNames = DeviceExtensions;

	VkResult Success = vkCreateDevice(PhysicalDevice, &CreateInfo, NULL, &Device);
	vkGetDeviceQueue(Device, Indices.GraphicsFamily, 0, &GraphicsQueue);
	vkGetDeviceQueue(Device, Indices.PresentFamily, 0, &PresentQueue);

	return Success;
}