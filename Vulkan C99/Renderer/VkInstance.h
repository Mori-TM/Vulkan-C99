VkResult CreateInstance()
{
	VkApplicationInfo AppInfo;
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pNext = 0;
	AppInfo.pApplicationName = "Vulkan Renderer";
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.pEngineName = "FRE";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_2;

	unsigned int ExtensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(Window, &ExtensionCount, NULL);
	const char** Extensions = malloc(ExtensionCount * sizeof(char*));
	SDL_Vulkan_GetInstanceExtensions(Window, &ExtensionCount, Extensions);

	for (int i = 0; i < ExtensionCount; i++)
		printf("Extension: %s\n", Extensions[i]);

	VkInstanceCreateInfo CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	CreateInfo.pNext = NULL;
	CreateInfo.flags = 0;
	CreateInfo.pApplicationInfo = &AppInfo;
	CreateInfo.enabledExtensionCount = ExtensionCount;
	CreateInfo.ppEnabledExtensionNames = Extensions;
	CreateInfo.enabledLayerCount = ValidationLayerCount;
	CreateInfo.ppEnabledLayerNames = ValidationLayers;

	return vkCreateInstance(&CreateInfo, NULL, &Instance);
}