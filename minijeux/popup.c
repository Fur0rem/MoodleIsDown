#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_ttf.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../intro.c"

/// Un minijeu qui harcèle le joueur avec des popups
/// Il ne peut y avoir que 5 popups à l'écran en même temps

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	char letter_to_press;
	float angle;
} Popup;

typedef struct {
	Popup** array;
	int size;
	int capacity;
} PopupArray;

void push(PopupArray* array, SDL_Window* window, SDL_Renderer* renderer,
	    char letter_to_press) {

	if (array->size == array->capacity) {
		// supprimer le premier élément
		SDL_DestroyWindow(array->array[0]->window);
		SDL_DestroyRenderer(array->array[0]->renderer);
		free(array->array[0]);

		// décaler les éléments
		for (int i = 0; i < array->size - 1; i++) {
			array->array[i] = array->array[i + 1];
		}

		// ajouter le nouvel élément
		Popup* p = (Popup*)malloc(sizeof(Popup));
		if (!p) {
			fprintf(stderr, "Error malloc");
			exit(4);
		}
		p->window = window;
		p->renderer = renderer;
		p->letter_to_press = letter_to_press;
		p->angle = 0;
		array->array[array->size - 1] = p;
	}

	else {
		Popup* p = (Popup*)malloc(sizeof(Popup));
		if (!p) {
			fprintf(stderr, "Error malloc");
			exit(4);
		}
		p->window = window;
		p->renderer = renderer;
		p->letter_to_press = letter_to_press;
		p->angle = 0;
		array->array[array->size] = p;
		array->size++;
	}
}

int popup(int* timer) {

	const int WIN_X = 1200;
	const int WIN_Y = 800;

	const int SMALL_WIN_X = 300;
	const int SMALL_WIN_Y = 200;

	SDL_Window* window =
	    SDL_CreateWindow("C McTeea", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	intro("popup", timer, renderer, window);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	// On crée des popups, chacun dans un endroit aléatoire de l'écran
	// On attend que le joueur appuie sur la touche correspondante à chaque popup

	const int NB_POPUPS = 10;
	const int MAX_POPUPS_AT_ONCE = 5;

	int success = 0;

	// Rendu de la lettre à presser sur la popup
	TTF_Font* font = TTF_OpenFont("fonts/stocky.ttf", 80);
	// SDL_Color red = {205, 0, 0, 255};
	SDL_Color white = {255, 255, 255, 255};

	PopupArray windows;
	windows.array = (Popup**)malloc(MAX_POPUPS_AT_ONCE * sizeof(Popup*));
	windows.size = 0;
	windows.capacity = MAX_POPUPS_AT_ONCE;

	while ((!success) && (*timer > 0)) {

		for (int i = 0; i < NB_POPUPS; i++) {

			char letter_to_press = 'a' + (rand() % 26);

			// On crée une popup²
			SDL_Window* popup =
			    SDL_CreateWindow("SAlut", rand() % WIN_X, rand() % WIN_Y,
						   SMALL_WIN_X, SMALL_WIN_Y, SDL_WINDOW_OPENGL);
			SDL_Renderer* popup_renderer = SDL_CreateRenderer(
			    popup, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

			char letter[2];
			letter[0] = toupper(letter_to_press);
			letter[1] = '\0';
			SDL_Surface* letter_surface = TTF_RenderText_Solid(font, letter, white);
			// SDL_FillRect(letter_surface, NULL, SDL_MapRGB(letter_surface->format,
			// red.r, red.g, red.b));
			SDL_Texture* letter_texture =
			    SDL_CreateTextureFromSurface(popup_renderer, letter_surface);
			SDL_Rect letter_rect = {SMALL_WIN_X / 2 - letter_surface->w / 2,
							SMALL_WIN_Y / 2 - letter_surface->h / 2,
							letter_surface->w, letter_surface->h};

			SDL_SetRenderDrawColor(popup_renderer, 255, 0, 0, 255);
			SDL_RenderClear(popup_renderer);
			SDL_RenderCopy(popup_renderer, letter_texture, NULL, &letter_rect);
			SDL_RenderPresent(popup_renderer);

			// On met la popup dans l'array
			push(&windows, popup, popup_renderer, letter_to_press);

			int index = (i < MAX_POPUPS_AT_ONCE) ? i : MAX_POPUPS_AT_ONCE - 1;

			// On attend que le joueur appuie sur la touche correspondante
			SDL_Event event;

			// Le joueur doit d'abord cliquer sur la popup pour pouvoir appuyer sur
			// la touche
			int can_type = 0;

			int new_success = 0;
			while ((!new_success) && (*timer > 0)) {
				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_KEYDOWN) {
						if (can_type) {
							if (event.key.keysym.sym ==
							    windows.array[index]->letter_to_press) {
								new_success = 1;
							}
						}
					}

					// Le joueur doit d'abord cliquer sur la popup pour pouvoir
					// appuyer sur la touche On prend la position de la souris
					else if (event.type == SDL_MOUSEBUTTONDOWN) {
						// On prend la position de la souris pour voir si
						// c'est la bonne popup
						int mouse_x = event.button.x;
						int mouse_y = event.button.y;
						if (mouse_x >= 0 && mouse_x <= SMALL_WIN_X &&
						    mouse_y >= 0 && mouse_y <= SMALL_WIN_Y) {
							can_type = 1;
						}
					}
				}

				// On bouge aléatoirement les popups
				for (int j = 0; j < windows.size; j++) {

					Popup* pop = windows.array[j];
					int x, y;
					SDL_GetWindowPosition(pop->window, &x, &y);
					pop->angle += (float)rand() / (RAND_MAX * 0.05);
					const int SPEED = 4;
					float delta_speed =
					    windows.size + 0.5 * (rand() / (RAND_MAX * 0.07));
					x += sinf(pop->angle) * SPEED * delta_speed;
					y += cosf(pop->angle) * SPEED * delta_speed;
					x = (x < 30) ? 30 : x;
					y = (y < 30) ? 30 : y;
					x = (x > 1920 - SMALL_WIN_X - 30) ? 1920 - SMALL_WIN_X - 30
										    : x;
					y = (y > 1080 - SMALL_WIN_Y - 30) ? 1080 - SMALL_WIN_Y - 30
										    : y;
					SDL_SetWindowPosition(pop->window, (x < 0) ? 0 : x,
								    (y < 0) ? 0 : y);
				}
			}
		}
		success = 1;
	}

	// On détruit les popups
	for (int i = 0; i < windows.size; i++) {
		SDL_DestroyWindow(windows.array[i]->window);
		SDL_DestroyRenderer(windows.array[i]->renderer);
		free(windows.array[i]);
	}
	free(windows.array);

	return success;
}