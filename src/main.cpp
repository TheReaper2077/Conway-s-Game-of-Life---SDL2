#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <iostream>	
#include <chrono>
#include <ratio>
#include <stdint.h>
#include <stdio.h>
#include <utility>
#include <vector>
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_video.h"
#include "assert.h"

#define WIDTH 1280
#define HEIGHT 640
#define TITLE "Conway's Game of Life"
#define FULLSCREEN false

#define ZOOM_IN SDLK_i
#define ZOOM_OUT SDLK_o
#define AUTOMATIC SDLK_SPACE
#define MOVE_UP SDLK_w
#define MOVE_DOWN SDLK_s
#define MOVE_RIGHT SDLK_d
#define MOVE_LEFT SDLK_a
#define STEP SDLK_e
#define RANDOMIZE SDLK_r
#define CLEAR SDLK_c
#define SHOW_GRID SDLK_v

#define LIVING_COLOR SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
#define GRID_COLOR SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
#define BACKGROUND_COLOR SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Event *event;
SDL_Rect rect;

enum BLOCK {
	DEAD, 
	LIVING
};

int gridsize = 2;
const int grid_width = WIDTH/2, grid_heigth = WIDTH/2;


bool isRunning;
double dt;
const int UPS = 60;
uint8_t grid[grid_heigth][grid_width];
bool mouse_pressed = false;

bool display_grid = false;
int posX = 0, posY = grid_heigth/2 - (grid_heigth/2)/2;

void select_block() {
	grid[(event->motion.y / gridsize) + posY][(event->motion.x / gridsize) + posX] = LIVING;
}

void randomize_grid() {
	for (int y = 0; y != sizeof(grid)/sizeof(grid[0]); y++) {
		for (int x = 0; x != sizeof(grid[y])/sizeof(grid[y][0]); x++) {
			grid[y][x] = BLOCK(rand()*17 % 2);
		}
	}
}

void clear_grid() {
	for (int y = 0; y != sizeof(grid)/sizeof(grid[0]); y++) {
		for (int x = 0; x != sizeof(grid[y])/sizeof(grid[y][0]); x++) {
			grid[y][x] = DEAD;
		}
	}
}

bool inRange(int x, int y) {
	if ((y < sizeof(grid)/sizeof(grid[0])) && (x < sizeof(grid[0])/sizeof(grid[0][0]) && x >= 0 && y >= 0)) {
		if (grid[y][x + 1] == LIVING &&  x != 0 && y != 0) {
			return true;
		}
	}

	return false;
}

void update() {
	uint8_t temp[grid_heigth][grid_width];

	for (int y = 0; y != sizeof(grid)/sizeof(grid[0]); y++) {
		for (int x = 0; x != sizeof(grid[y])/sizeof(grid[y][0]); x++) {
			int living_neighbours = 0;

			for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 1; i++) {
					living_neighbours += grid[y + j][x + i];
				}
			}

			living_neighbours -= grid[y][x];

			if (living_neighbours == 3 && grid[y][x] == DEAD) {
				temp[y][x] = LIVING;
			} else if (living_neighbours < 2 && grid[y][x] == LIVING) {
				temp[y][x] = DEAD;
			} else if (living_neighbours > 3 && grid[y][x] == LIVING) {
				temp[y][x] = DEAD;
			} else {
				temp[y][x] = grid[y][x];
			}
		}
	}
	
	for (int y = 0; y != sizeof(grid)/sizeof(grid[0]); y++) {
		for (int x = 0; x != sizeof(grid[y])/sizeof(grid[y][0]); x++) {
			grid[y][x] = temp[y][x];
		}
	}
}

void draw_grid() {
	for (int y = 0; y != grid_heigth; y++) {
		for (int x = 0; x != grid_width; x++) {
			rect = {x*gridsize, y*gridsize, gridsize, gridsize};
			if (grid[y + posY][x + posX] == LIVING) {
				SDL_SetRenderDrawColor(renderer, 0, rand() % 255, rand() % 255, 255);
				SDL_RenderFillRect(renderer, &rect);
			}

			if (display_grid) {
				GRID_COLOR;
				SDL_RenderDrawRect(renderer, &rect);
			}
		}
	}
}

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, (FULLSCREEN) ? SDL_WINDOW_FULLSCREEN: SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	assert(window && renderer);

	isRunning = true;
	event = new SDL_Event();
}

int main(int argc, char **argv) {
	init();
	// grid[0 + 2][1 + 2] = grid[1 + 2][2 + 2] = grid[2 + 2][2 + 2] = grid[2 + 2][1 + 2] = grid[2 + 2][0 + 2] = LIVING;
	float count = 0;
	bool auto_m = false;

	while (isRunning) {
		const auto& t_start = std::chrono::high_resolution_clock::now();
		
		while (SDL_PollEvent(event)) {
			if (event->type == SDL_QUIT) {
				isRunning = false;
			}
			if (event->type == SDL_KEYDOWN) {
				switch (event->key.keysym.sym) {
					case STEP:
						update();
						break;
					case RANDOMIZE:
						randomize_grid();
						break;
					case CLEAR:
						clear_grid();
						break;
					case ZOOM_IN:
						gridsize++;
						break;
					case ZOOM_OUT:
						gridsize--;
						break;
					case AUTOMATIC:
						auto_m = !auto_m;
						break;
					case SHOW_GRID:
						display_grid = !display_grid;
						break;
					case MOVE_UP:
						if (posY != 0) posY--;
						break;
					case MOVE_DOWN:
						posY++;
						break;
					case MOVE_LEFT:
						if (posX != 0) posX--;
						break;
					case MOVE_RIGHT:
						posX++;
						break;
				}
			}
			if (event->type == SDL_MOUSEBUTTONDOWN) {
				select_block();
			}
		}

		if (mouse_pressed) {
			select_block();
		}

		BACKGROUND_COLOR;
		SDL_RenderClear(renderer);
		draw_grid();
		SDL_RenderPresent(renderer);

		const auto& t_end = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<double, std::ratio<1, UPS>>(t_end - t_start).count();
		count += dt;
		
		if (count > 1 && auto_m) {
			update();
			count = 0;
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}