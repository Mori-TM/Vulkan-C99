VkResult CreateImageViews()
{
	SwapChainImageViews = malloc(SwapChainImageViewCount * sizeof(VkImageView));

	for (unsigned int i = 0; i < SwapChainImageViewCount; i++)
		SwapChainImageViews[i] = CreateImageView(SwapChainImages[i], SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	return VK_SUCCESS;
}