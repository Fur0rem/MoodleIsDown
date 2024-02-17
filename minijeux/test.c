#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>

#include <stdio.h>
#include <stdlib.h>

#include "../intro.c"

int test(int* timer) {

    const int WIN_X = 1200;
    const int WIN_Y = 800;

    SDL_Window* window = SDL_CreateWindow("Game Jam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_X, WIN_Y, SDL_WINDOW_OPENGL);
    if (!window) {
        fprintf(stderr,"Error window creation");
        exit(2);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr,"Error renderer creation");
        exit(3);
    }

    intro("test", timer, renderer, window);

    SDL_Event event;
    int success = 0;
    while ((!success) && (*timer > 0)) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        printf("UP\n");
                        success = 1;
                    case SDLK_DOWN:
                        printf("DOWN\n");
                        break;
                    default:
                        break;
                }
            } 
            //else if (event.type == SDL_KEYUP) {

            //}

            // Le rendu
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

        }
        //printf("%d\n", *timer);
    }

    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

    return success;
}