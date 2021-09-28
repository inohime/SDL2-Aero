/*
Created by: Inohime @ 9/26/2021
*/

#include <SDL.h>
#include <SDL_syswm.h>
#include <dwmapi.h>
#include <iostream>

#pragma comment (lib, "dwmapi.lib")

constexpr int W = 1024;
constexpr int H = 768;

struct _position
{
	SDL_Rect client { 0, 0, W, H };
}Pos;

void SetShadow(SDL_Window *win, HWND hwnd)
{
	// sets the shadow on the borderless window
	int v = 2;
	DwmSetWindowAttribute(hwnd, 2, &v, 4);

	MARGINS margins
	{
		margins.cxLeftWidth = 0,
		margins.cxRightWidth = 0,
		margins.cyTopHeight = 0,
		margins.cyBottomHeight = 1
	};

	DwmExtendFrameIntoClientArea(hwnd, &margins);
}

SDL_HitTestResult Result(SDL_Window *window, const SDL_Point *pt, void *data)
{
	if (SDL_PointInRect(pt, &Pos.client))
	{
		//SDL_Log("HIT-TEST: DRAGGABLE\n");
		return SDL_HITTEST_DRAGGABLE;
	}

	return SDL_HITTEST_NORMAL;
}

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << "Failed to initialize SDL" << std::endl;
		return -1;
	}

	SDL_SetHintWithPriority("SDL_BORDERLESS_RESIZABLE_STYLE", "1", SDL_HINT_OVERRIDE);
	SDL_SetHintWithPriority("SDL_BORDERLESS_WINDOWED_STYLE", "1", SDL_HINT_OVERRIDE);

	SDL_Window *window = SDL_CreateWindow(
		"lol",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		W,
		H,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED);

	if (SDL_SetWindowHitTest(window, Result, NULL) == -1) {
		SDL_Log("Enabling hit-testing failed!\n");
		SDL_Quit();
		return 1;
	}

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;

	SetShadow(window, hwnd);

	bool quit = false;

	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
				break;
			}
			break;
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// for outlining the area to drag
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &Pos.client);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}