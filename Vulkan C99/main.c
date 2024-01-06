#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
//#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

size_t AllocatedCPUBytes = 0;

void* AllocateMem(size_t Size)
{
	AllocatedCPUBytes += Size;

	return malloc(Size);
}


void* ReAllocateMem(void* Block, size_t Size)
{
	AllocatedCPUBytes += Size;

	return realloc(Block, Size);
}

#define malloc AllocateMem
#define realloc ReAllocateMem

void SandboxInit();
void SandboxUpdate();
void SandboxRenderDepth();
void SandboxRender();
void SandboxRenderGUI();
void SandboxDeInit();

#include "WaveAudio.h"
#include "WaveLoader.h"
#include "FileDialog.h"
#include "Defines.h"
#include "Vector.h"
#include "Structs.h"
#include "Helper.h"
#include "GUI.h"
#include "Sandbox.h"
#include "Renderer.h"

int ResizingEventWatcher(void* Data, SDL_Event* Event) 
{
	if (Event->type == SDL_WINDOWEVENT && Event->window.event == SDL_WINDOWEVENT_RESIZED)
	{
		SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
		if (Window == (SDL_Window*)Data)
		{
			SDL_GetWindowSize(Window, &Width, &Height);
			RecreateSwapChain();

			CalcFps("FRE");

			DrawFrame();
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	system("GLSLCompiler.bat");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
//	SDL_ShowCursor(SDL_DISABLE);

	Window = SDL_CreateWindow("FRE Engine", 0, 30, Width, Height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	SDL_SetWindowMinimumSize(Window, 1280, 720);

	InitVulkan();
	printf("Validation Layers %s\n", ValidationLayerCount == 1 ? "enabled" : "disabled");

	SandboxInit();
	
	printf("Allocated Mega Bytes: %f\n", (double)AllocatedCPUBytes / 1048576);
	bool PlayAudio = false;
//	WaveAudio Audio;
//	WaveLoadAudio("Mask Off Remix.wav", &Audio);
//	WavePlayAudio(&Audio, PlayAudio);
	SDL_AddEventWatch(ResizingEventWatcher, Window);

	VertexConst.Model = LoadMat4Identity();
	VertexConst.View = LoadMat4Identity();
	VertexConst.Proj = LoadMat4Identity();
	FragmentConst.UseTextures = true;
	FragmentConst.LightBrightness = 1.0;
	FragmentConst.FilterLevel = 0;
	GUIFragmentConst.Display = 1;
	GUIFragmentConst.Color = Vec4(1, 1, 1, 1);
	BlurFragmentConst.Horizontal = false;

	bool Run = true;
	bool FullScreen = false;

	while (Run)
	{
		while (SDL_PollEvent(&Event))
		{
			GUIPollEvent();

			if (Event.type == SDL_QUIT)
				Run = false;

			if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_ESCAPE)
				Run = false;

			if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_p)
			{
				PlayAudio = !PlayAudio;
			//	WavePlayAudio(&Audio, PlayAudio);
			}

			if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_m)
			{
				LoadModel = false;
				LoadModelFromFile();
			}

			if (Event.type == SDL_KEYDOWN && Event.key.keysym.sym == SDLK_F11)
			{
				SDL_SetWindowFullscreen(Window, (FullScreen = !FullScreen) ? SDL_WINDOW_FULLSCREEN_DESKTOP : false);
				//	SDL_SetWindowSize(Window, 1920, 1080);
				//	FullScreen = !FullScreen;
				//	SDL_GetWindowSize(Window, &Width, &Height);
				//	RecreateSwapChain();
				//	GUIResize();
			}

			if (Event.window.event == SDL_WINDOWEVENT_RESIZED || Event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				SDL_GetWindowSize(Window, &Width, &Height);
				RecreateSwapChain();
				printf("Resized\n");
			}
			
		}

		if (LoadModel)
		{
			LoadModel = false;
			LoadModelFromFile();
		}

		if (LoadTexture)
		{
			LoadTexture = false;
			vkDeviceWaitIdle(Device);
			char Path[2048];
			if (OpenDialog(Path, ALLIMAGES))
			{
				OrangeTexture = CreateTextureImage(Path, true);

				CreateTextureDescriptorSet(true, &TextureDescriptorSets[OrangeTexture], &TextureDescriptorSetLayout, TextureSampler[OrangeTexture], TextureImageViews[OrangeTexture], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0);
			}
		}

		CalcFps("FRE");

		DrawFrame();
	}

	SDL_SetWindowFullscreen(Window, false);
//	WaveFreeAudio(&Audio);
	DestroyVulkan();
	SDL_DestroyWindow(Window);
	SDL_Quit();

	return 1;
}