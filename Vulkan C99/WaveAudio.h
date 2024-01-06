#include <stdio.h>

typedef struct
{
	unsigned int Length;
	unsigned char* Buffer;
	SDL_AudioSpec Spectrum;
	SDL_AudioDeviceID ID;
} WaveAudio;

int WaveLoadAudio(const char* Path, WaveAudio* Data)
{
	SDL_LoadWAV(Path, &Data->Spectrum, &Data->Buffer, &Data->Length);
	Data->ID = SDL_OpenAudioDevice(NULL, 0, &Data->Spectrum, NULL, 0);
	int Success = SDL_QueueAudio(Data->ID, Data->Buffer, Data->Length);

	return Success;
}

void WavePlayAudio(WaveAudio* Data, int Play)
{
	SDL_PauseAudioDevice(Data->ID, !Play);
}

void WaveFreeAudio(WaveAudio* Data)
{
	SDL_CloseAudioDevice(Data->ID);
	SDL_FreeWAV(Data->Buffer);
}