/*
Created by: Inohime @ 9/26/2021
*/

#include <SDL.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include <dwmapi.h>
#include <iostream>

#pragma comment (lib, "dwmapi.lib")

constexpr int W = 1024;
constexpr int H = 768;

struct _pos
{
	SDL_Rect rect { 0, 0, W, H };
}Pos;

void SetBorderless(SDL_Window *win, HWND hwnd)
{
	// remove border
	SDL_SetWindowBordered(win, SDL_FALSE);
	int v = 2;

	// sets the shadow on the borderless window
	// https://docs.microsoft.com/en-us/windows/win32/api/dwmapi/nf-dwmapi-dwmsetwindowattribute
	DwmSetWindowAttribute(hwnd, 2, &v, 4);
	MARGINS margins { margins.cxLeftWidth = 0, margins.cxRightWidth = 0, margins.cyTopHeight = 0, margins.cyBottomHeight = 1 };
	DwmExtendFrameIntoClientArea(hwnd, &margins);
}

void SetMaximized(SDL_Window *win)
{
	SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

// hit test to drag the window while borderless
SDL_HitTestResult Result(SDL_Window *window, const SDL_Point *pt, void *data)
{
	if (SDL_PointInRect(pt, &Pos.rect))
	{
		SDL_Log("HIT-TEST: DRAGGABLE\n");
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

	SDL_Window *window = SDL_CreateWindow("lol", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_ALWAYS_ON_TOP);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// get the window handle
	SDL_SysWMinfo WMinfo;
	SDL_VERSION(&WMinfo.version);
	SDL_GetWindowWMInfo(window, &WMinfo);
	HWND hwnd = (HWND) WMinfo.info.win.window;

	if (SDL_SetWindowHitTest(window, Result, NULL) == -1) {
		SDL_Log("Enabling hit-testing failed!\n");
		SDL_Quit();
		return 1;
	}

	bool quit = false;

	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				std::cout << "focus gained\n";
				break;

			case SDL_WINDOWEVENT_MOVED:
				SDL_Log("Window moved to (%d, %d)!\n", (int) e.window.data1, (int) e.window.data2);
				break;

			case SDL_WINDOWEVENT_FOCUS_LOST:
				std::cout << "focus lost\n";
				break;
			}
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE: { quit = true;		break; }
				case SDLK_F9: { SetMaximized(window);  break; }
				case SDLK_F8: { SetBorderless(window, hwnd); break; }
				}
				break;
			}
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// for outlining the area to drag
		//SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		//SDL_RenderDrawRect(renderer, &Pos.rect);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}