#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <iostream>	
#include <chrono>
#include <ratio>
#include <stdint.h>
#include <stdio.h>
#include <unordered_set>
#include <utility>
#include <vector>
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_video.h"
#include "assert.h"

#define WIDTH 1280
#define HEIGHT 640
#define TITLE "Conway's Game of Life"

#define LIVING_COLOR SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
#define GRID_COLOR SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
#define BACKGROUND_COLOR SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Event *event;
SDL_Rect rect;

enum BLOCK {
	DEAD, 
	LIVING
};

const int grid_width = WIDTH/4, grid_heigth = HEIGHT/4;

bool isRunning;
double dt;
const int UPS = 60;
uint8_t grid[grid_heigth][grid_width];
// std::vector<std::vector<uint8_t>> grid;
int gridsize = 16;

void randomize_grid() {
	for (int y = 0; y != sizeof(grid)/sizeof(grid[0]); y++) {
		for (int x = 0; x != sizeof(grid[y])/sizeof(grid[y][0]); x++) {
			grid[y][x] = BLOCK(rand() % 2);
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
	for (int y = 0; y != sizeof(grid)/sizeof(grid[0]); y++) {
		for (int x = 0; x != sizeof(grid[y])/sizeof(grid[y][0]); x++) {
			rect = {x*gridsize, y*gridsize, gridsize, gridsize};
			if (grid[y][x] == LIVING) {
				LIVING_COLOR;
				SDL_RenderFillRect(renderer, &rect);
			}

			GRID_COLOR;
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
}

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	assert(window && renderer);

	isRunning = true;
	event = new SDL_Event();
}

int main(int argc, char **argv) {
	init();
	grid[0 + 2][1 + 2] = grid[1 + 2][2 + 2] = grid[2 + 2][2 + 2] = grid[2 + 2][1 + 2] = grid[2 + 2][0 + 2] = LIVING;
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
					case SDLK_e:
						update();
						break;
					case SDLK_r:
						randomize_grid();
						break;
					case SDLK_c:
						clear_grid();
						break;
					case SDLK_i:
						gridsize++;
						break;
					case SDLK_d:
						gridsize--;
						break;
					case SDLK_s:
						auto_m = !auto_m;
						break;
				}
			}
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