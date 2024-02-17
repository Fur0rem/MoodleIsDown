#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>

#include <stdio.h>
#include <stdlib.h>

int flipper() {

    const int WIN_X = 1200;
    const int WIN_Y = 800;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr,"Error SDL2 Initialization : %s",SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game Jam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_X, WIN_Y, SDL_WINDOW_OPENGL);
    if (!window) {
        fprintf(stderr,"Error window creation");
        return 2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr,"Error renderer creation");
        return 3;
    }

    SDL_Event event;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } 
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        printf("UP\n");
                        break;
                    case SDLK_DOWN:
                        printf("DOWN\n");
                        break;
                    default:
                        break;
                }
            } 
            else if (event.type == SDL_KEYUP) {

            }
        }
    }
}