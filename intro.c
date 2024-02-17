#ifndef INTRO_C
#define INTRO_C

/// Affiche les règles du jeu et attend que le joueur appuie sur une touche pour commencer

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_thread.h>

// #include "intro.h"

// Affiche les règles du jeu sur la fenêtre SDL
void intro(const char* name_of_minigame, int* timer, SDL_Renderer* renderer,
	     SDL_Window* window) {

	SDL_Event event;
	int ready = 0;
	while ((!ready) && (*timer > 0)) {

		// L'image à afficher se trouve dans fichiers/intros/name_of_minigame.png
		char path_to_image[100] = "fichiers/intros/";
		strcat(path_to_image, name_of_minigame);
		strcat(path_to_image, ".png");

		// printf("%s\n", path_to_image);

		// Affichage de l'image
		SDL_Surface* image = IMG_Load(path_to_image);
		if (!image) {
			fprintf(stderr, "Error image loading : %s", SDL_GetError());
			exit(1);
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		// Gestion des évènements (on attend que le joueur appuie sur ENTREE)
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_RETURN:
					ready = 1;
					break;
				default:
					break;
				}
			}
		}

		SDL_DestroyTexture(texture);
		SDL_FreeSurface(image);
	}
}

#endif