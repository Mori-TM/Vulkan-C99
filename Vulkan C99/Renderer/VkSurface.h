VkResult CreateSurface()
{
	return !SDL_Vulkan_CreateSurface(Window, Instance, &Surface);
}