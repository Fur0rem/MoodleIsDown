/// Le joueur doit drag la fenetre du souffleur pour dépoussiérer sa box

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_thread.h>

// #include "../FastNoiseLite.h"

#include "../perlin.c"

#include <stdio.h>
#include <stdlib.h>

#include "../intro.c"

SDL_Texture* texture_box;

void render_poussiere(SDL_Renderer* renderer, int** poussieres, int x, int y,
			    int poussiere_size) {

	SDL_RenderCopy(renderer, texture_box, NULL, NULL);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {

			if (poussieres[i][j]) {
				SDL_Rect rect = {i * poussiere_size, j * poussiere_size,
						     poussiere_size, poussiere_size};
				int opacity = 70 * poussieres[i][j];
				opacity = opacity > 255 ? 255 : opacity;
				SDL_SetRenderDrawColor(renderer, 127, 127, 127, opacity);
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

void populate_pousseires(int** poussieres, int x, int y) {
	// On remplit le tableau de poussières avec un bruit de Perlin
	// On utilise la fonction de bruit de Perlin de la librairie FastNoise

	// Create and configure noise state

	// Gather noise data
	float* noise_data = malloc(sizeof(float) * x * y);
	if (!noise_data) {
		fprintf(stderr, "Error malloc");
		exit(4);
	}

	for (int i = 0; i < x * y; i++) {
		noise_data[i] =
		    (perlin2d(i % x, i / x, 0.1, 1) + perlin2d(i % x, i / x, 0.5, 2) +
		     perlin2d(i % x, i / x, 9.0, 10)) /
		    6.0;
	}

	// On normalise les données
	float min = 0;
	float max = 0;
	for (int i = 0; i < x * y; i++) {
		if (noise_data[i] < min) {
			min = noise_data[i];
		}
		if (noise_data[i] > max) {
			max = noise_data[i];
		}
	}

	for (int i = 0; i < x * y; i++) {
		noise_data[i] = (noise_data[i] - min) / (max - min);
	}

	// On remplit le tableau de poussières
	for (int i = 0; i < x * y; i++) {
		poussieres[i % x][i / x] = noise_data[i] * 4;
	}

	free(noise_data);
}

const int SMALL_WIN_X = 200;
const int SMALL_WIN_Y = 150;

void depoussierer(SDL_Renderer* renderer, int** poussieres, int x, int y,
			int poussiere_size, SDL_Window* small_window) {
	// Position du souffleur
	int small_window_x, small_window_y;
	SDL_GetWindowPosition(small_window, &small_window_x, &small_window_y);

	small_window_y -= SMALL_WIN_Y * 2;
	small_window_x -= SMALL_WIN_X;

	// On décale les poussières soufflées (à proximité du souffleur) vers la gauche
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {

			// On vérifie si la poussière est à proximité du souffleur
			int blow_strength = 0; // 4 = max, 0 = rien
			if (i * poussiere_size > small_window_x &&
			    i * poussiere_size < small_window_x + SMALL_WIN_X) {
				if (j * poussiere_size > small_window_y &&
				    j * poussiere_size < small_window_y + SMALL_WIN_Y) {
					blow_strength =
					    6 - (i * poussiere_size - small_window_x) / 75;
					blow_strength = blow_strength > 6 ? 6 : blow_strength;
					blow_strength = blow_strength < 0 ? 0 : blow_strength;
				}
			}

			// On décale la poussière vers la droite
			if (blow_strength) {
				if (i < x - 1) {
					poussieres[i + 1][j] += poussieres[i][j];
					if (poussieres[i + 1][j] > 4) {
						poussieres[i + 1][j] = 4;
					}
				}
				poussieres[i][j] -= blow_strength;
				if (poussieres[i][j] < 0) {
					poussieres[i][j] = 0;
				}
			}
		}
	}
}

int has_won_poussiere(int** poussieres, int x, int y) {
	int won = 1;
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y && won; j++) {
			if (poussieres[i][j]) {
				won = 0;
			}
		}
	}
	return won;
}

int souffleur(int* timer) {

	const int WIN_X = 1000;
	const int WIN_Y = 700;

	SDL_Window* window =
	    SDL_CreateWindow("SOUFFLE!!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				   WIN_X, WIN_Y, SDL_WINDOW_OPENGL);
	if (!window) {
		fprintf(stderr, "Error window creation");
		exit(2);
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(
	    window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		fprintf(stderr, "Error renderer creation");
		exit(3);
	}

	intro("souffleur", timer, renderer, window);

	// on met le fond d'une box internet
	SDL_Surface* background = IMG_Load("fichiers/souffleur/box_pascontente.png");
	if (!background) {
		fprintf(stderr, "Erreur box internet");
		exit(2);
	}
	texture_box = SDL_CreateTextureFromSurface(renderer, background);
	SDL_RenderCopy(renderer, texture_box, NULL, NULL);
	SDL_RenderPresent(renderer);

	// On crée la petite fenêtre du souffleur
	SDL_Window* small_window = SDL_CreateWindow(
	    "Serge le seche-cheveux", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	    SMALL_WIN_X, SMALL_WIN_Y, SDL_WINDOW_OPENGL);
	if (!small_window) {
		fprintf(stderr, "Error window creation");
		exit(2);
	}
	SDL_Renderer* small_renderer = SDL_CreateRenderer(
	    small_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_SetWindowPosition(small_window, 0, 0);
	// Mettre le seche cheveux sur la fenetre
	SDL_Surface* background_seche = IMG_Load("fichiers/souffleur/seche_cheveux.png");
	if (!background) {
		fprintf(stderr, "Erreur seche cheveux");
		exit(2);
	}
	SDL_Texture* texture_seche =
	    SDL_CreateTextureFromSurface(small_renderer, background_seche);
	SDL_RenderCopy(small_renderer, texture_seche, NULL, NULL);
	SDL_RenderPresent(small_renderer);

	// On crée le tableau de poussières

	const int POUSSIERE_SIZE = 10;

	int nb_poussieres_x = WIN_X / POUSSIERE_SIZE;
	int nb_poussieres_y = WIN_Y / POUSSIERE_SIZE;

	int** poussieres = (int**)malloc(sizeof(int*) * nb_poussieres_x);
	if (!poussieres) {
		fprintf(stderr, "Error malloc");
		exit(4);
	}
	for (int i = 0; i < nb_poussieres_x; i++) {
		poussieres[i] = malloc(sizeof(int) * nb_poussieres_y);
		if (!poussieres[i]) {
			fprintf(stderr, "Error malloc");
			exit(4);
		}
	}

	populate_pousseires(poussieres, nb_poussieres_x, nb_poussieres_y);

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
		}
		depoussierer(renderer, poussieres, nb_poussieres_x, nb_poussieres_y,
				 POUSSIERE_SIZE, small_window);

		// Le rendu
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		render_poussiere(renderer, poussieres, nb_poussieres_x, nb_poussieres_y,
				     POUSSIERE_SIZE);
		SDL_RenderPresent(renderer);

		success = has_won_poussiere(poussieres, nb_poussieres_x, nb_poussieres_y);
	}

	if (success) {
		SDL_Surface* box_happy = IMG_Load("fichiers/souffleur/box_contente.png");
		if (!box_happy) {
			fprintf(stderr, "Erreur box internet");
			exit(2);
		}
		texture_box = SDL_CreateTextureFromSurface(renderer, box_happy);
		SDL_Surface* etoile = IMG_Load("fichiers/souffleur/etoile.png");
		if (!etoile) {
			fprintf(stderr, "Erreur etoile");
			exit(2);
		}
		SDL_Texture* texture_etoile = SDL_CreateTextureFromSurface(renderer, etoile);

		int coordinates[4][2] = {{125, 76}, {569, 200}, {100, 500}, {800, 300}};

		// on fait briller la box
		int i = 0;
		for (int i1 = 0; i1 < 100; i1++) {
			if (i1 % 5 == 0) {
				i += 5;
			}
			// on affiche la box contente
			SDL_RenderCopy(renderer, texture_box, NULL, NULL);

			// on affiche 4 étoile qui grandissent et rapetissent
			for (int j = 0; j < 4; j++) {
				int size = 3 * abs((i % 30) - 15) * (j + 1) + 80;
				SDL_Rect rect = {coordinates[j][0], coordinates[j][1], size,
						     size};
				SDL_RenderCopy(renderer, texture_etoile, NULL, &rect);
			}

			SDL_RenderPresent(renderer);
		}

		SDL_DestroyTexture(texture_etoile);
		SDL_FreeSurface(etoile);
		SDL_DestroyTexture(texture_box);
		SDL_FreeSurface(box_happy);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(small_renderer);
	SDL_DestroyWindow(small_window);

	return success;
}