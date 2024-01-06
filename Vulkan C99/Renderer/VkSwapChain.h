VkResult CreateSwapChain()
{
	SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(PhysicalDevice);

	VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats, SwapChainSupport.FormatCount);
	VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes, SwapChainSupport.PresentModeCount);
	VkExtent2D Extent = ChooseSwapExtent(SwapChainSupport.Capabilities);

	unsigned int ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;
	if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
		ImageCount = SwapChainSupport.Capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.surface = Surface;
	CreateInfo.minImageCount = ImageCount;
	CreateInfo.imageFormat = SurfaceFormat.format;
	CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
	CreateInfo.imageExtent = Extent;
	CreateInfo.imageArrayLayers = 1;
	CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices Indices = FindQueueFamilies(PhysicalDevice);
	unsigned int FamilyIndices[] = { Indices.GraphicsFamily, Indices.PresentFamily };

	//Don't know why to use VK_SHARING_MODE_CONCURRENT cause VK_SHARING_MODE_EXCLUSIVE is faster
	if (Indices.GraphicsFamily != Indices.PresentFamily)
	{
		CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		CreateInfo.queueFamilyIndexCount = 2;
		CreateInfo.pQueueFamilyIndices = FamilyIndices;
	}
	else
	{
		CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount = 0;
		CreateInfo.pQueueFamilyIndices = NULL;
	}

	CreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
	CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	CreateInfo.presentMode = PresentMode;
	CreateInfo.clipped = VK_TRUE;
	CreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(Device, &CreateInfo, NULL, &SwapChain) != VK_SUCCESS)
		return 1;

	vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, NULL);
	SwapChainImages = malloc(ImageCount * sizeof(VkImage));
	SwapChainImageViewCount = ImageCount;
	vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, SwapChainImages);

	SwapChainImageFormat = SurfaceFormat.format;
	SwapChainExtent = Extent;

	return VK_SUCCESS;
}