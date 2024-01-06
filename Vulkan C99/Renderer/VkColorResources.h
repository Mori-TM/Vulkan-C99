VkResult CreateOffScreenColorResources(unsigned int Index, unsigned int ColorAttachments, bool Msaa)
{
	VkFormat ColorFormat = SwapChainImageFormat;

	for (unsigned int i = 0; i < ColorAttachments; i++)
	{
		if (CreateImage(OffScreenPass[Index].Width, OffScreenPass[Index].Height, 1, VK_SAMPLE_COUNT_1_BIT, ColorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &OffScreenPass[Index].ColorImage[i], &OffScreenPass[Index].ColorImageMemory[i]) != VK_SUCCESS)
			return 1;
		OffScreenPass[Index].ColorImageView[i] = CreateImageView(OffScreenPass[Index].ColorImage[i], ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	
		if (Msaa)
		{
			if (CreateImage(OffScreenPass[Index].Width, OffScreenPass[Index].Height, 1, MsaaSamples, ColorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &OffScreenPass[Index].MsaaColorImage[i], &OffScreenPass[Index].MsaaColorImageMemory[i]) != VK_SUCCESS)
				return 1;
			OffScreenPass[Index].MsaaColorImageView[i] = CreateImageView(OffScreenPass[Index].MsaaColorImage[i], ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}
	
	return VK_SUCCESS;
}

VkResult CreateColorResources()
{
	if (CreateOffScreenColorResources(1, 2, true) != VK_SUCCESS) return 1;
	if (CreateOffScreenColorResources(2, 1, false) != VK_SUCCESS) return 1;
	if (CreateOffScreenColorResources(3, 1, false) != VK_SUCCESS) return 1;

	return VK_SUCCESS;
}