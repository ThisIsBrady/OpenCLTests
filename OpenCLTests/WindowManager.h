#pragma once
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include <string>
class WindowManager
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
public:
	WindowManager();
	~WindowManager();
	void updateScreen(void const* buffer, int pitch);
	bool process();
	void updateTitle(const std::string& title);
};

