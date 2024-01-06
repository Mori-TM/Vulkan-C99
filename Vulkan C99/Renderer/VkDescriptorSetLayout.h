VkResult CreateDescriptorSetLayout(unsigned int Binding, VkDescriptorType DescriptorType, unsigned int BindingCount, VkShaderStageFlagBits ShaderType, VkDescriptorSetLayout* DescriptorSetLayout)
{
	VkDescriptorSetLayoutBinding* LayoutBindings = malloc(BindingCount * sizeof(VkDescriptorSetLayoutBinding));
	for (unsigned int i = 0; i < BindingCount; i++)
	{
		LayoutBindings[i].binding = Binding + i;
		LayoutBindings[i].descriptorType = DescriptorType;
		LayoutBindings[i].descriptorCount = 1;
		LayoutBindings[i].stageFlags = ShaderType;
		LayoutBindings[i].pImmutableSamplers = NULL;
	}
	

	VkDescriptorSetLayoutCreateInfo LayoutInfo;
	LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	LayoutInfo.pNext = NULL;
	LayoutInfo.flags = 0;
	LayoutInfo.bindingCount = BindingCount;
	LayoutInfo.pBindings = LayoutBindings;

	return vkCreateDescriptorSetLayout(Device, &LayoutInfo, NULL, DescriptorSetLayout);
}

VkResult CreateDescriptorSetLayouts()
{	
	//Descriptor Set Layout for the Default Vertex Shader
	if (CreateDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, &UniformDescriptorSetLayout) != VK_SUCCESS) return 1;
	//Descriptor Set Layout for the Default Fragment Shader to use Textures
	if (CreateDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &TextureDescriptorSetLayout) != VK_SUCCESS) return 1;

	//Descriptor Set Layout for the Light Depth Fragment Shader
	if (CreateDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &OffScreenPass[0].DescriptorSetLayout) != VK_SUCCESS) return 1;

	//Descriptor Set Layout for the Default Fragment Shader for the Texture Output 
	if (CreateDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, VK_SHADER_STAGE_FRAGMENT_BIT, &OffScreenPass[1].DescriptorSetLayout) != VK_SUCCESS) return 1;

	//Descriptor Set Layout for the Horizontal and Vertical Blur
	if (CreateDescriptorSetLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &OffScreenPass[2].DescriptorSetLayout) != VK_SUCCESS) return 1;
	if (CreateDescriptorSetLayout(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, &OffScreenPass[3].DescriptorSetLayout) != VK_SUCCESS) return 1;

	return VK_SUCCESS;
}