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

struct _position {
	SDL_Rect dragRect {W / 4, H / 4, W / 2, H / 2};
	SDL_Rect client {0, 0, W, H};
	SDL_Rect topLeft {0, 0, W / 12, H / 12};
	SDL_Rect topRight {static_cast<int>(W / 1.09), 0, W / 12, H / 12};
	SDL_Rect bottomLeft {0, static_cast<int>(H / 1.09), W / 12, H / 12};
	SDL_Rect bottomRight{static_cast<int>(W / 1.09), static_cast<int>(H / 1.09), W / 12, H / 12};
}Pos;

void SetShadow(SDL_Window *win, HWND hwnd) {
	// sets the shadow on the borderless window
	int v = 2;
	DwmSetWindowAttribute(hwnd, 2, &v, 4);

	MARGINS margins {
		margins.cxLeftWidth = 0,
		margins.cxRightWidth = 0,
		margins.cyTopHeight = 0,
		margins.cyBottomHeight = 1
	};

	DwmExtendFrameIntoClientArea(hwnd, &margins);
}

SDL_HitTestResult Result(SDL_Window *window, const SDL_Point *pt, void *data) {
	if (SDL_PointInRect(pt, &Pos.dragRect))
		return SDL_HITTEST_DRAGGABLE;

	if (SDL_PointInRect(pt, &Pos.topLeft))
		return SDL_HITTEST_RESIZE_TOPLEFT;

	if (SDL_PointInRect(pt, &Pos.topRight))
		return SDL_HITTEST_RESIZE_TOPRIGHT;

	if (SDL_PointInRect(pt, &Pos.bottomLeft))
		return SDL_HITTEST_RESIZE_BOTTOMLEFT;

	if (SDL_PointInRect(pt, &Pos.bottomRight))
		return SDL_HITTEST_RESIZE_BOTTOMRIGHT;

	return SDL_HITTEST_NORMAL;
}

void DrawRect(const SDL_Color &color, SDL_Renderer* ren, const SDL_Rect *rect) {
	SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(ren, rect);
}

int main(int argc, char *argv[]) 
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
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
		SDL_WINDOW_RESIZABLE | 
		SDL_WINDOW_BORDERLESS);

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

	while (!quit) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_WINDOWEVENT:
				switch (e.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					Pos.client.w = e.window.data1;
					Pos.client.h = e.window.data2;

					int winw, winh;
					SDL_GetWindowSize(window, &winw, &winh);

					// to show resize works
					Pos.topRight.x = static_cast<int>((Pos.client.w / 2.00) * 1.85);

					Pos.bottomLeft.y = static_cast<int>((Pos.client.h / 2.00) * 1.85);

					Pos.bottomRight.x = static_cast<int>((Pos.client.w / 2.00) * 1.85);
					Pos.bottomRight.y = static_cast<int>((Pos.client.h / 2.00) * 1.85);
					break;
				}
				break;

			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				}
				break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		DrawRect({255, 166, 195, 255}, renderer, &Pos.topLeft);
		DrawRect({255, 166, 195, 255}, renderer, &Pos.topRight);
		DrawRect({255, 166, 195, 255}, renderer, &Pos.bottomLeft);
		DrawRect({255, 166, 195, 255}, renderer, &Pos.bottomRight);
		//DrawRect({255, 166, 195, 255}, renderer, &Pos.dragRect);
		DrawRect({255, 166, 195, 255}, renderer, &Pos.client);

		SDL_RenderSetViewport(renderer, &Pos.client);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}