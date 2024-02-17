#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "minijeux/defonce_ta_tour.c"
#include "minijeux/disjoncteur.c"
#include "minijeux/labyrinthe.c"
#include "minijeux/popup.c"
#include "minijeux/souffleur.c"
#include "minijeux/test.c"

Uint32 timer_callback(Uint32 interval, void* timer) {
	*(int*)timer -= 1;
	return interval;
}

const int MAX_TIMER = 500000;

/// This is the documentation for the thing function.
/// It does nothing bla bla bla bla bla bla bla bla bla bla bla bla bla bla.
/// @param thing3: the thing3 parameter
/// @return: 0 + thing3
int thing(int thing3) {
	return 0 + thing3;
}

int main() {

	thing(3);

	srand(time(NULL));
	// Tous les minijeux sous forme d'array de pointeurs de fonctions
	// Les fonctions retournent 0 si le joueur a perdu, 1 si il a gagné
	// Les fonctions prennent en paramètre un pointeur vers le timer

	const int NB_MINIJEUX = 5;
	int (*minijeux[NB_MINIJEUX])(int*);
	minijeux[0] = &souffleur;
	minijeux[1] = &disjoncteur;
	minijeux[2] = &labyrinthe;

	minijeux[3] = &popup;
	minijeux[4] = &defonce_ta_tour;
	// minijeux[3] = &defonce_ta_tour;

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error SDL2 Initialization : %s", SDL_GetError());
		exit(1);
	}

	if (TTF_Init() < 0) {
		fprintf(stderr, "Error TTF Initialization : %s", SDL_GetError());
		exit(1);
	}

	// On crée la fenêtre de résumé
	const int WIN_X = 1200;
	const int WIN_Y = 800;

	SDL_Window* window =
	    SDL_CreateWindow("Game Jam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	// On crée la fenêtre de résumé
	SDL_Surface* menu = IMG_Load("fichiers/MOODLE_IS_DOWN-MENU.png");
	if (!menu) {
		fprintf(stderr, "Erreur resum");
		exit(2);
	}

	SDL_Texture* texture_menu = SDL_CreateTextureFromSurface(renderer, menu);
	SDL_RenderCopy(renderer, texture_menu, NULL, NULL);
	SDL_RenderPresent(renderer);

	// On attend que l'utilisateur appuie sur une touche
	SDL_Event event;
	int success = 0;
	while (!success) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_RETURN:
					success = 1;
					break;
				default:
					break;
				}
			}
			// else if (event.type == SDL_KEYUP) {

			//}
		}
	}

	// On crée la fenêtre de résumé
	SDL_Surface* panne = IMG_Load("fichiers/panne_internet.png");
	if (!panne) {
		fprintf(stderr, "Erreur resum");
		exit(2);
	}

	SDL_Texture* texture_panne = SDL_CreateTextureFromSurface(renderer, panne);
	SDL_RenderCopy(renderer, texture_panne, NULL, NULL);
	SDL_RenderPresent(renderer);

	// On attend que l'utilisateur appuie sur une touche
	success = 0;
	while (!success) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_RETURN:
					success = 1;
					break;
				default:
					break;
				}
			}
			// else if (event.type == SDL_KEYUP) {

			//}
		}
	}

	printf("%d, %d\n", WIN_X, WIN_Y);

	// On crée la fenêtre de résumé
	SDL_Surface* background = IMG_Load("fichiers/resum.png");
	if (!background) {
		fprintf(stderr, "Erreur resum");
		exit(2);
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, background);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	// On attend que l'utilisateur appuie sur une touche
	success = 0;
	while (!success) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_RETURN:
					success = 1;
					break;
				default:
					break;
				}
			}
			// else if (event.type == SDL_KEYUP) {

			//}
		}
	}
	// destruction de la fenêtre de résumé
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	int timer = MAX_TIMER;
	SDL_TimerID id = SDL_AddTimer(10, timer_callback, &timer);

	for (int i = 0; i < NB_MINIJEUX;) {
		int result = minijeux[i](&timer);
		if (result) {
			i++;
		}
		else {
			goto fail;
		}
	}
	// win:
	//  On load les 5 convs whatsapp
	SDL_Surface* conv1 = IMG_Load("fichiers/fin/whatsapp1.jpg");
	if (!conv1) {
		fprintf(stderr, "Erreur whatsapp1");
		exit(2);
	}
	SDL_Surface* conv2 = IMG_Load("fichiers/fin/whatsapp2.jpg");
	if (!conv2) {
		fprintf(stderr, "Erreur whatsapp2");
		exit(2);
	}
	SDL_Surface* conv3 = IMG_Load("fichiers/fin/whatsapp3.jpg");
	if (!conv3) {
		fprintf(stderr, "Erreur whatsapp3");
		exit(2);
	}
	SDL_Surface* conv4 = IMG_Load("fichiers/fin/whatsapp4.jpg");
	if (!conv4) {
		fprintf(stderr, "Erreur whatsapp4");
		exit(2);
	}
	SDL_Surface* conv5 = IMG_Load("fichiers/fin/whatsapp5.jpg");
	if (!conv5) {
		fprintf(stderr, "Erreur whatsapp5");
		exit(2);
	}

	// On crée la fenêtre de résumé
	SDL_Window* window2 =
	    SDL_CreateWindow("Game Jam", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				   300, 800, SDL_WINDOW_OPENGL);
	if (!window2) {
		fprintf(stderr, "Error window creation");
		exit(2);
	}

	SDL_Renderer* renderer2 = SDL_CreateRenderer(
	    window2, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	SDL_Surface* convs[5] = {conv1, conv2, conv3, conv4, conv5};

	// On load les 5 convs whatsapp
	for (int i = 0; i < 5; i++) {
		SDL_Texture* texture_conv = SDL_CreateTextureFromSurface(renderer2, convs[i]);
		SDL_RenderCopy(renderer2, texture_conv, NULL, NULL);
		SDL_RenderPresent(renderer2);
		SDL_Event event2;

		int success2 = 0;
		while (!success2) {
			while (SDL_PollEvent(&event2)) {
				if (event2.type == SDL_KEYDOWN) {
					switch (event2.key.keysym.sym) {
					case SDLK_RETURN:
						success2 = 1;
						break;
					default:
						break;
					}
				}
				// else if (event.type == SDL_KEYUP) {

				//}
			}
		}
		SDL_DestroyTexture(texture_conv);
		SDL_FreeSurface(convs[i]);
	}

	// on affiche l'écran de fin
	SDL_Surface* fin = IMG_Load("fichiers/fin/fin.png");
	if (!fin) {
		fprintf(stderr, "Erreur fin");
		exit(2);
	}

	SDL_SetWindowSize(window2, 1200, 800);
	SDL_SetWindowPosition(window2, 0, 0);

	SDL_Texture* texture_fin = SDL_CreateTextureFromSurface(renderer2, fin);
	SDL_RenderCopy(renderer2, texture_fin, NULL, NULL);
	SDL_RenderPresent(renderer2);

	// On attend que l'utilisateur appuie sur une touche
	success = 0;
	while (!success) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_RETURN:
					success = 1;
					break;
				default:
					break;
				}
			}
			// else if (event.type == SDL_KEYUP) {

			//}
		}
	}

	SDL_DestroyRenderer(renderer2);
	SDL_DestroyWindow(window2);

	printf("Vous avez gagné !\n");
	printf("Vous avez pris %d secondes\n", (MAX_TIMER - timer) / 100);
	exit(0);

fail:
	printf("Vous avez perdu !\n");
	printf("Vous avez tenu %d secondes\n", timer);
	exit(0);
}