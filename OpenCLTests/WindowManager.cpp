#include "WindowManager.h"
WindowManager::WindowManager()
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("OpenCL Stuff", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, 800, 600);
}

WindowManager::~WindowManager()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void WindowManager::updateScreen(void const* buffer, int pitch)
{
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool WindowManager::process()
{
	bool quit = false;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
	}
	return quit;
}

void WindowManager::updateTitle(const std::string& title)
{
	SDL_SetWindowTitle(window, title.data());
}