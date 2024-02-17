#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include "../intro.c"

typedef struct {
	int health;
	int stage;
} Tour;

void shake_window(SDL_Window* window) {
	int x, y;
	SDL_GetWindowPosition(window, &x, &y);
	for (int i = 0; i < 8; i++) {
		int rand_x = 15 * (rand() % 10 - 5);
		int rand_y = 15 * (rand() % 10 - 5);
		SDL_SetWindowPosition(window, x + rand_x, y + rand_y);
		SDL_Delay(1);
	}
	SDL_SetWindowPosition(window, x, y);
}

int defonce_ta_tour(int* timer) {

	int WIN_X = 1200;
	int WIN_Y = 900;

	SDL_Window* window =
	    SDL_CreateWindow("DEFONCE TA TOUR", SDL_WINDOWPOS_CENTERED,
				   SDL_WINDOWPOS_CENTERED, WIN_X, WIN_Y, SDL_WINDOW_OPENGL);
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

	intro("defonce_ta_tour", timer, renderer, window);
	const int HEALTH = 20;
	const int STAGE = HEALTH / 4;
	Tour tour = {.health = HEALTH, .stage = 0};

	WIN_X = 500;
	WIN_Y = 900;

	SDL_SetWindowSize(window, WIN_X, WIN_Y);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_Event event;

	SDL_Surface* computer_stage0 = IMG_Load("fichiers/tour/stage0.png");
	if (!computer_stage0) {
		fprintf(stderr, "Erreur tour");
		exit(2);
	}
	SDL_Surface* computer_stage1 = IMG_Load("fichiers/tour/stage1.png");
	SDL_Surface* computer_stage2 = IMG_Load("fichiers/tour/stage2.png");
	SDL_Surface* computer_stage3 = IMG_Load("fichiers/tour/stage3.png");

	SDL_Texture* computer_texture_stage0 =
	    SDL_CreateTextureFromSurface(renderer, computer_stage0);
	SDL_Texture* computer_texture_stage1 =
	    SDL_CreateTextureFromSurface(renderer, computer_stage1);
	SDL_Texture* computer_texture_stage2 =
	    SDL_CreateTextureFromSurface(renderer, computer_stage2);
	SDL_Texture* computer_texture_stage3 =
	    SDL_CreateTextureFromSurface(renderer, computer_stage3);

	SDL_Rect computer_rect = {.x = 0, .y = 0, .w = WIN_X, .h = WIN_Y};

	SDL_Surface* gagne = IMG_Load("fichiers/tour/gagne.png");

	int success = 0;
	while ((!success) && (*timer > 0)) {
		while (SDL_PollEvent(&event)) {

			// Si tu cliques sur la fenetre, la tour perd de la vie
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				tour.health--;
				shake_window(window);
			}
		}

		// On change le stage de la tour
		tour.health = tour.health < 0 ? 0 : tour.health;
		tour.stage = 4 - (tour.health / STAGE);

		SDL_Texture* text_affiche = NULL;
		if (tour.stage == 0) {
			text_affiche = computer_texture_stage0;
		}
		else if (tour.stage == 1) {
			text_affiche = computer_texture_stage1;
		}
		else if (tour.stage == 2) {
			text_affiche = computer_texture_stage2;
		}
		else if (tour.stage == 3) {
			text_affiche = computer_texture_stage3;
		}
		else {
			text_affiche = SDL_CreateTextureFromSurface(renderer, gagne);
			success = 1;
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, text_affiche, NULL, &computer_rect);

		SDL_RenderPresent(renderer);

		// printf("%d\n", *timer);
	}

	sleep(2);

	SDL_DestroyTexture(computer_texture_stage0);
	SDL_DestroyTexture(computer_texture_stage1);
	SDL_DestroyTexture(computer_texture_stage2);
	SDL_DestroyTexture(computer_texture_stage3);
	SDL_FreeSurface(computer_stage0);
	SDL_FreeSurface(computer_stage1);
	SDL_FreeSurface(computer_stage2);
	SDL_FreeSurface(computer_stage3);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	const int PLAYER_SIZE = 150;

	// Phase 2 : boss (pong)

	// Affiche le boss
	WIN_X = 150;
	WIN_Y = 400;

	SDL_Window* boss_window =
	    SDL_CreateWindow("BOSS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_X,
				   WIN_Y, SDL_WINDOW_OPENGL);
	if (!boss_window) {
		fprintf(stderr, "Error window boss");
		exit(2);
	}

	SDL_SetWindowPosition(boss_window, 1500, SDL_WINDOWPOS_CENTERED);

	SDL_Renderer* boss_renderer = SDL_CreateRenderer(
	    boss_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!boss_renderer) {
		fprintf(stderr, "Error renderer boss");
		exit(3);
	}

	SDL_Surface* boss = IMG_Load("fichiers/tour/boss.png");
	if (!boss) {
		fprintf(stderr, "Erreur boss");
		exit(2);
	}

	SDL_Texture* boss_texture = SDL_CreateTextureFromSurface(boss_renderer, boss);
	if (!boss_texture) {
		fprintf(stderr, "Erreur texture boss");
		exit(2);
	}

	SDL_Rect boss_rect = {.x = 0, .y = 0, .w = WIN_X, .h = WIN_Y};

	SDL_RenderCopy(boss_renderer, boss_texture, NULL, &boss_rect);
	SDL_RenderPresent(boss_renderer);

	SDL_Window* player_window =
	    SDL_CreateWindow("PLAYER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 70,
				   PLAYER_SIZE, SDL_WINDOW_OPENGL);
	if (!player_window) {
		fprintf(stderr, "Error window player");
		exit(2);
	}

	const int PLAYER_OFFSET = 200;
	SDL_SetWindowPosition(player_window, PLAYER_OFFSET, SDL_WINDOWPOS_CENTERED);

	SDL_Renderer* player_renderer = SDL_CreateRenderer(
	    player_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!player_renderer) {
		fprintf(stderr, "Error renderer player");
		exit(3);
	}

	SDL_Surface* player = IMG_Load("fichiers/tour/player.png");
	if (!player) {
		fprintf(stderr, "Erreur player");
		exit(2);
	}

	SDL_Texture* player_texture = SDL_CreateTextureFromSurface(player_renderer, player);
	if (!player_texture) {
		fprintf(stderr, "Erreur texture player");
		exit(2);
	}

	SDL_Rect player_rect = {.x = 0, .y = 0, .w = 70, .h = PLAYER_SIZE};

	SDL_RenderCopy(player_renderer, player_texture, NULL, &player_rect);
	SDL_RenderPresent(player_renderer);

	// On popup une fenetre avec le texte
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_Window* popup =
	    SDL_CreateWindow("Ceci est un dialogue", 727, SDL_WINDOWPOS_CENTERED,
				   727 /*WYSI*/, 200, SDL_WINDOW_OPENGL);
	SDL_Renderer* popup_renderer = SDL_CreateRenderer(
	    popup, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	TTF_Font* font = TTF_OpenFont("fonts/Spooky_Stone.ttf", 50);
	if (!font) {
		fprintf(stderr, "Erreur font");
		exit(2);
	}
	SDL_Color orange = {255, 165, 0, 255};

	const char* dialogues[] = {"Je suis le boss.",	"Un peu nul a tout",
					   "A part a pong",	"Je suis tres fort a pong",
					   "Me juge pas",		"Tu vas voir ce que c'est",
					   "Que de me hagraah", "Jeune insolent de merde"};

	for (int i = 0; i < 8; i++) {
		SDL_SetRenderDrawColor(popup_renderer, 0, 0, 0, 255);
		SDL_RenderClear(popup_renderer);
		SDL_Surface* dialogue_surface =
		    TTF_RenderText_Solid(font, dialogues[i], orange);
		SDL_Texture* dialogue_texture =
		    SDL_CreateTextureFromSurface(popup_renderer, dialogue_surface);
		SDL_Rect dialogue_rect = {0, 0, dialogue_surface->w, dialogue_surface->h};
		SDL_RenderCopy(popup_renderer, dialogue_texture, NULL, &dialogue_rect);
		SDL_RenderPresent(popup_renderer);
		SDL_DestroyTexture(dialogue_texture);
		SDL_FreeSurface(dialogue_surface);
		SDL_SetWindowInputFocus(popup);
		SDL_Delay(300);
		SDL_SetWindowInputFocus(player_window);
		SDL_Delay(300);
		SDL_SetWindowInputFocus(boss_window);
		SDL_Delay(300);
	}

	SDL_DestroyRenderer(popup_renderer);
	SDL_DestroyWindow(popup);

	SDL_SetWindowInputFocus(player_window);

	int player_health = 3;
	int boss_health = 3;

	// Crée la balle (dans sa propre fenetre)
	SDL_Window* ball_window = SDL_CreateWindow(
	    "O", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 50, 50, SDL_WINDOW_OPENGL);
	if (!ball_window) {
		fprintf(stderr, "Error window ball");
		exit(2);
	}

	SDL_Renderer* ball_renderer = SDL_CreateRenderer(
	    ball_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (!ball_renderer) {
		fprintf(stderr, "Error renderer ball");
		exit(3);
	}

	SDL_Surface* ball = IMG_Load("fichiers/tour/boule.png");
	if (!ball) {
		fprintf(stderr, "Erreur ball");
		exit(2);
	}

	SDL_Texture* ball_texture = SDL_CreateTextureFromSurface(ball_renderer, ball);
	if (!ball_texture) {
		fprintf(stderr, "Erreur texture ball");
		exit(2);
	}

	SDL_Rect ball_rect = {.x = 0, .y = 0, .w = 50, .h = 50};

	SDL_RenderCopy(ball_renderer, ball_texture, NULL, &ball_rect);
	SDL_RenderPresent(ball_renderer);

	int ball_x = 1920 / 2 - 25;
	int ball_y = 1080 / 2 - 25;

	SDL_SetWindowPosition(ball_window, ball_x, ball_y);

	double deltaTime = 0;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;

	float ball_dx = -8.0;
	float ball_dy = 8.0;

	// On lance le jeu de pong
	success = 0;
	int player_y = 0;
	int boss_y;
	int bounced = 0;
	// re center the mouse
	SDL_WarpMouseInWindow(player_window, 150, 500);

	// switch to opacity blend
	SDL_SetRenderDrawBlendMode(player_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawBlendMode(boss_renderer, SDL_BLENDMODE_BLEND);

	SDL_GetWindowPosition(boss_window, NULL, &boss_y);
	while ((!success) && (*timer > 0)) {

		// re center the mouse
		SDL_WarpMouseInWindow(player_window, 150, 500);

		while (SDL_PollEvent(&event)) {
			// on prend la pos de la souris
			if (event.type == SDL_MOUSEMOTION) {
				// get mouse position
				int mouse_y = event.motion.yrel;
				player_y += mouse_y;
				if (player_y < 100) {
					player_y = 100;
				}
				if (player_y > 900) {
					player_y = 900;
				}
			}
		}

		// on met à jour la position du joueur
		SDL_SetWindowPosition(player_window, PLAYER_OFFSET + 150, player_y);
		SDL_SetWindowPosition(boss_window, 1500, boss_y);

		// on bouge le boss
		boss_y += (int)(ball_y - boss_y) / 40;
		if (boss_y < 50) {
			boss_y = 50;
		}
		if (boss_y > 1080 - 450 - 5) {
			boss_y = 1080 - 450 - 5;
		}

		ball_x += ball_dx;
		ball_y += ball_dy;

		// rebondir sur le haut et le bas
		if ((!bounced) && (ball_y < 120 || ball_y > 1080 - 70)) {
			ball_dy = -ball_dy;
			bounced = 1;
		}
		else {
			bounced = 0;
		}

		// rebondir sur le joueur si on est à la bonne position
		if (ball_x < PLAYER_OFFSET + 200) {
			int rect_y_up = player_y - 70;
			int rect_y_down = player_y + PLAYER_SIZE + 70;
			// printf("%d %d %d\n", ball_y, rect_y_up, rect_y_down);
			if (ball_y > rect_y_up && ball_y < rect_y_down) {
				// on calcule l'angle de rebond
				float angle =
				    (float)(ball_y - rect_y_up) / (float)(PLAYER_SIZE + 70);
				// printf("%f\n", angle);
				ball_dx = 8 * cosf(angle * 3.14159265 / 2);
				ball_dy = 8 * sinf(angle * 3.14159265 / 2);
			}
			else {
				// le joueur perd de la vie
				player_health--;
				shake_window(player_window);
				if (player_health == 0) {
					success = 0;
					break;
				}
				// on rend le joueur un peu plus rouge (on change la couleur de la
				// fenetre) draw a red rectangle inside of the windo with opacity
				// 0.5
				SDL_SetRenderDrawColor(player_renderer, 255, 0, 0,
							     80 * (3 - player_health));
				// draw the player
				SDL_RenderClear(player_renderer);
				SDL_RenderCopy(player_renderer, player_texture, NULL,
						   &player_rect);

				SDL_Rect rect = {0, 0, 70, PLAYER_SIZE};
				SDL_RenderFillRect(player_renderer, &rect);
				SDL_RenderPresent(player_renderer);

				SDL_Delay(1000);
				ball_x = 1920 / 2 - 25;
				ball_y = 1080 / 2 - 25;
				ball_dx = -8.0;
			}
		}

		// rebondir sur le boss si on est à la bonne position
		if (ball_x > 1500 - 50) {
			int rect_y_up = boss_y - 70;
			int rect_y_down = boss_y + 500;
			// printf("%d %d %d\n", ball_y, rect_y_up, rect_y_down);
			if (ball_y > rect_y_up && ball_y < rect_y_down) {
				// on calcule l'angle de rebond
				float angle = (float)(ball_y - rect_y_up) / (float)(500);
				// printf("%f\n", angle);
				ball_dx = -8 * cosf(angle * 3.14159265 / 2);
				ball_dy = 8 * sinf(angle * 3.14159265 / 2);
			}
			else {
				// le boss perd de la vie
				boss_health--;
				shake_window(boss_window);
				if (boss_health == 0) {
					success = 1;
				}
				// on rend le boss un peu plus rouge (on change la couleur de la
				// fenetre) draw a red rectangle inside of the windo with opacity
				// 0.5
				SDL_SetRenderDrawColor(boss_renderer, 255, 0, 0,
							     80 * (3 - boss_health));
				// draw the player
				SDL_RenderClear(boss_renderer);
				SDL_RenderCopy(boss_renderer, boss_texture, NULL, &boss_rect);

				SDL_Rect rect = {0, 0, 150, 400};
				SDL_RenderFillRect(boss_renderer, &rect);
				SDL_RenderPresent(boss_renderer);

				SDL_Delay(1000);
				ball_x = 1920 / 2 - 25;
				ball_y = 1080 / 2 - 25;
				ball_dx = -8.0;
			}
		}

		if (abs((int)ball_dx) < 5) {
			ball_dx *= 1.02;
		}

		SDL_SetWindowPosition(ball_window, ball_x, ball_y);
		SDL_SetWindowPosition(player_window, PLAYER_OFFSET + 150, player_y);
	}

	SDL_DestroyTexture(boss_texture);
	SDL_DestroyTexture(player_texture);
	SDL_DestroyTexture(ball_texture);
	SDL_FreeSurface(boss);
	SDL_FreeSurface(player);
	SDL_FreeSurface(ball);

	SDL_DestroyRenderer(boss_renderer);
	SDL_DestroyWindow(boss_window);
	SDL_DestroyRenderer(player_renderer);
	SDL_DestroyWindow(player_window);
	SDL_DestroyRenderer(ball_renderer);
	SDL_DestroyWindow(ball_window);

	return success;
}