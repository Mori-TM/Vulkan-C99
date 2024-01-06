VkResult CreateImage(unsigned int Width, unsigned int Height, unsigned int MipLevels, VkSampleCountFlagBits NumSamples, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Usage, VkMemoryPropertyFlags Properties, VkImage* Image, VkDeviceMemory* ImageMemory)
{
	VkImageCreateInfo ImageInfo;
	ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageInfo.pNext = NULL;
	ImageInfo.flags = 0;
	ImageInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageInfo.format = Format;
	ImageInfo.extent.width = Width;
	ImageInfo.extent.height = Height;
	ImageInfo.extent.depth = 1;
	ImageInfo.mipLevels = MipLevels;
	ImageInfo.arrayLayers = 1;
	ImageInfo.samples = NumSamples;
	ImageInfo.tiling = Tiling;
	ImageInfo.usage = Usage;
	ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ImageInfo.queueFamilyIndexCount = 0;
	ImageInfo.pQueueFamilyIndices = NULL;
	ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(Device, &ImageInfo, NULL, Image) != VK_SUCCESS)
		return 1;

	VkMemoryRequirements MemoryRequirements;
	vkGetImageMemoryRequirements(Device, *Image, &MemoryRequirements);

	VkMemoryAllocateInfo AllocateInfo;
	AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	AllocateInfo.pNext = NULL;
	AllocateInfo.allocationSize = MemoryRequirements.size;
	AllocateInfo.memoryTypeIndex = FindMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(Device, &AllocateInfo, NULL, ImageMemory) != VK_SUCCESS)
		return 1;

	vkBindImageMemory(Device, *Image, *ImageMemory, 0);

	return VK_SUCCESS;
}

VkResult TransitionImageLayout(VkImage Image, VkFormat Format, VkImageLayout OldLayout, VkImageLayout NewLayout, unsigned int MipLevels)
{
	VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier Barrier;
	Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	Barrier.pNext = NULL;
	Barrier.srcAccessMask = 0;
	Barrier.dstAccessMask = 0;
	Barrier.oldLayout = OldLayout;
	Barrier.newLayout = NewLayout;
	Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.image = Image;
	Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	Barrier.subresourceRange.baseMipLevel = 0;
	Barrier.subresourceRange.levelCount = MipLevels;
	Barrier.subresourceRange.baseArrayLayer = 0;
	Barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags SourceStage;
	VkPipelineStageFlags DestinationStage;

	if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		Barrier.srcAccessMask = 0;
		Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		DestinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
		return 1;

	vkCmdPipelineBarrier(CommandBuffer, SourceStage, DestinationStage, 0, 0, NULL, 0, NULL, 1, &Barrier);

	EndSingleTimeCommandBuffer(CommandBuffer);

	return VK_SUCCESS;
}

void CopyBufferToImage(VkBuffer Buffer, VkImage Image, unsigned int Width, unsigned int Height)
{
	VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

	VkBufferImageCopy Region;
	Region.bufferOffset = 0;
	Region.bufferRowLength = 0;
	Region.bufferImageHeight = 0;
	Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	Region.imageSubresource.mipLevel = 0;
	Region.imageSubresource.baseArrayLayer = 0;
	Region.imageSubresource.layerCount = 1;
	Region.imageOffset.x = 0;
	Region.imageOffset.y = 0;
	Region.imageOffset.z = 0;
	Region.imageExtent.width = Width;
	Region.imageExtent.height = Height;
	Region.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(CommandBuffer, Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);

	EndSingleTimeCommandBuffer(CommandBuffer);
}

void GenerateMipmaps(VkImage Image, VkFormat ImageFormat, unsigned int TextureWidth, unsigned int TextureHeight, unsigned int MipLevels)
{
	VkFormatProperties FormatProperties;
	vkGetPhysicalDeviceFormatProperties(PhysicalDevice, ImageFormat, &FormatProperties);
	if (!(FormatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		return 1;

	VkCommandBuffer CommandBuffer = BeginSingleTimeCommands();

	VkImageMemoryBarrier Barrier;
	Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	Barrier.pNext = NULL;
	Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	Barrier.image = Image;
	Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	Barrier.subresourceRange.levelCount = 1;
	Barrier.subresourceRange.baseArrayLayer = 0;
	Barrier.subresourceRange.layerCount = 1;

	int MipWidth = TextureWidth;
	int MipHeight = TextureHeight;

	for (unsigned int i = 1; i < MipLevels; i++)
	{
		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		Barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		Barrier.subresourceRange.baseMipLevel = i - 1;

		vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &Barrier);

		VkImageBlit Blit;
		
		Blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Blit.srcSubresource.mipLevel = i - 1;
		Blit.srcSubresource.baseArrayLayer = 0;
		Blit.srcSubresource.layerCount = 1;
		Blit.srcOffsets[0].x = 0;
		Blit.srcOffsets[0].y = 0;
		Blit.srcOffsets[0].z = 0;
		Blit.srcOffsets[1].x = MipWidth;
		Blit.srcOffsets[1].y = MipHeight;
		Blit.srcOffsets[1].z = 1;
		Blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Blit.dstSubresource.mipLevel = i;
		Blit.dstSubresource.baseArrayLayer = 0;
		Blit.dstSubresource.layerCount = 1;
		Blit.dstOffsets[0].x = 0;
		Blit.dstOffsets[0].y = 0;
		Blit.dstOffsets[0].z = 0;
		Blit.dstOffsets[1].x = MipWidth > 1 ? MipWidth / 2 : 1;
		Blit.dstOffsets[1].y = MipHeight > 1 ? MipHeight / 2 : 1;
		Blit.dstOffsets[1].z = 1;

		vkCmdBlitImage(CommandBuffer, Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Blit, VK_FILTER_LINEAR);

		Barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &Barrier);

		if (MipWidth > 1) MipWidth /= 2;
		if (MipHeight > 1) MipHeight /= 2;
	}

	Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	Barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	Barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	Barrier.subresourceRange.baseMipLevel = MipLevels - 1;

	vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &Barrier);

	EndSingleTimeCommandBuffer(CommandBuffer);
}

void CreateTextureImageView(VkImageView* TextureImageViews)
{
	*TextureImageViews = CreateImageView(TextureImages[TextureImageCount], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, MipLevels);
}

unsigned int CreateTextureImage(const char* Path, bool FlipVertical)
{
	TextureImageMemory = realloc(TextureImageMemory, (TextureImageCount + 1) * sizeof(VkDeviceMemory));
	TextureImages = realloc(TextureImages, (TextureImageCount + 1) * sizeof(VkImage));
	TextureImageViews = realloc(TextureImageViews, (TextureImageCount + 1) * sizeof(VkImageView));
	TextureSampler = realloc(TextureSampler, (TextureImageCount + 1) * sizeof(VkSampler));

	unsigned int TextureWidth;
	unsigned int TextureHeight;
	unsigned int TextureChannels;
	
	stbi_set_flip_vertically_on_load(FlipVertical);
	unsigned char* Pixel = stbi_load(Path, &TextureWidth, &TextureHeight, &TextureChannels, STBI_rgb_alpha);
	MipLevels = floorf(log2f(MAX(TextureWidth, TextureHeight))) + 1;

	VkDeviceSize ImageSize = TextureWidth * TextureHeight * 4;
	if (!Pixel)
		printf("Failed to Load Texture\n");

	VkBuffer StagingBuffer;
	VkDeviceMemory StagingBufferMemory;

	if (CreateBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &StagingBuffer, &StagingBufferMemory) != VK_SUCCESS)
		printf("Failed to create Texture Buffer\n");

	void* Data;
	vkMapMemory(Device, StagingBufferMemory, 0, ImageSize, 0, &Data);
	memcpy(Data, Pixel, ImageSize);
	vkUnmapMemory(Device, StagingBufferMemory);

	free(Pixel);

	if (CreateImage(TextureWidth, TextureHeight, MipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &TextureImages[TextureImageCount], &TextureImageMemory[TextureImageCount]) != VK_SUCCESS)
		printf("Failed to create Texture Image\n");

	if (TransitionImageLayout(TextureImages[TextureImageCount], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, MipLevels) != VK_SUCCESS)
		printf("Failed to create Texture\n");
	CopyBufferToImage(StagingBuffer, TextureImages[TextureImageCount], TextureWidth, TextureHeight);
//	if (TransitionImageLayout(TextureImages, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, MipLevels) != VK_SUCCESS)
//		return 1;
	GenerateMipmaps(TextureImages[TextureImageCount], VK_FORMAT_R8G8B8A8_SRGB, TextureWidth, TextureHeight, MipLevels);

	vkDestroyBuffer(Device, StagingBuffer, NULL);
	vkFreeMemory(Device, StagingBufferMemory, NULL);

	CreateTextureImageView(&TextureImageViews[TextureImageCount]);

	if (CreateTextureSampler(&TextureSampler[TextureImageCount], MipLevels, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT) != VK_SUCCESS)
		printf("Faild To Create Texture Sampler\n");

	TextureDescriptorSetsCount++;
	TextureImageCount++;

	return TextureImageCount - 1;
}