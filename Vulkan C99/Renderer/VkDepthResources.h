bool HasStencilComponent(VkFormat Format)
{
	return Format == VK_FORMAT_D32_SFLOAT || Format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkResult CreateOffScreenDepthResources(unsigned int Index, bool Msaa)
{
	VkFormat DepthFormat = FindDepthFormat();

	if (CreateImage(OffScreenPass[Index].Width, OffScreenPass[Index].Height, 1, Msaa ? MsaaSamples : VK_SAMPLE_COUNT_1_BIT, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &OffScreenPass[Index].DepthImage, &OffScreenPass[Index].DepthImageMemory) != VK_SUCCESS)
		return 1;
	OffScreenPass[Index].DepthImageView = CreateImageView(OffScreenPass[Index].DepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	return VK_SUCCESS;
}

VkResult CreateDepthResources()
{
	VkFormat DepthFormat = FindDepthFormat();

	if (CreateImage(SwapChainExtent.width, SwapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &DepthImage, &DepthImageMemory) != VK_SUCCESS)
		return 1;
	DepthImageView = CreateImageView(DepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	if (CreateOffScreenDepthResources(0, false) != VK_SUCCESS) return 1;
	if (CreateOffScreenDepthResources(1, true)  != VK_SUCCESS) return 1;

	return VK_SUCCESS;
}