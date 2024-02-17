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

#include "../intro.c"

const int CELL_SIZE = 25;

enum LabyrintheCellType { PATH, START, END };

typedef struct {
	enum LabyrintheCellType type;
	int walls[4];
} LabyrintheCell;

typedef struct {
	int width;
	int height;
	LabyrintheCell** cells;
} Labyrinthe;

typedef struct {
	int x;
	int y;
} Player;

int randint(int min, int max) {
	if (min >= max) {
		return min;
	}
	return rand() % (max - min) + min;
}

int countSetBits(int n) {
	unsigned int count = 0;
	while (n) {
		n &= (n - 1);
		count++;
	}
	return count;
}

int rand_int_normal_distribution(int min, int max) {
	int x = rand() % (max - min) + min;
	int y = rand() % (max - min) + min;
	return (x + y) / 2;
}

void subdivise(Labyrinthe* labyrinthe, int min_x, int max_x, int min_y, int max_y,
		   int x_or_y) {

	int width = max_x - min_x;
	int height = max_y - min_y;

	if ((width < 2) && (height < 2)) {
		return;
	}

	// On choisit si on crée un mur en x ou en y

	// int x_or_y = (width+height) % 2;
	if (width < 2) {
		x_or_y = 0;
	}
	else if (height < 2) {
		x_or_y = 1;
	}

	if (x_or_y) {
		// on crée un mur en x
		int x = rand_int_normal_distribution(min_x, max_x - 1);

		// Créer un mur à la frontière x
		for (int i = min_y; i < max_y; i++) {
			labyrinthe->cells[i * labyrinthe->width + x]->walls[1] = 1;
			labyrinthe->cells[i * labyrinthe->width + x + 1]->walls[3] = 1;
		}

		// on fait un trou dans le mur
		int y = randint(min_y, max_y);

		labyrinthe->cells[y * labyrinthe->width + x]->walls[1] = 0;
		labyrinthe->cells[y * labyrinthe->width + x + 1]->walls[3] = 0;

		// on subdivise les 2 sous carrés
		int rand = randint(0, 2);
		if (rand == 0) {
			subdivise(labyrinthe, min_x, x, min_y, max_y, 1);
			subdivise(labyrinthe, x + 1, max_x, min_y, max_y, 0);
		}
		else {
			subdivise(labyrinthe, x + 1, max_x, min_y, max_y, 0);
			subdivise(labyrinthe, min_x, x, min_y, max_y, 1);
		}
	}

	else {
		// on crée un mur en y
		int y = rand_int_normal_distribution(min_y, max_y - 1);

		// Créer un mur à la frontière y
		for (int i = min_x; i < max_x; i++) {
			labyrinthe->cells[y * labyrinthe->width + i]->walls[2] = 1;
			labyrinthe->cells[(y + 1) * labyrinthe->width + i]->walls[0] = 1;
		}

		// on fait un trou dans le mur
		int x = randint(min_x, max_x);
		labyrinthe->cells[y * labyrinthe->width + x]->walls[2] = 0;
		labyrinthe->cells[(y + 1) * labyrinthe->width + x]->walls[0] = 0;

		// on subdivise les 2 sous carrés
		int rand = randint(0, 2);
		if (rand == 0) {
			subdivise(labyrinthe, min_x, max_x, min_y, y, 0);
			subdivise(labyrinthe, min_x, max_x, y + 1, max_y, 1);
		}
		else {
			subdivise(labyrinthe, min_x, max_x, y + 1, max_y, 1);
			subdivise(labyrinthe, min_x, max_x, min_y, y, 0);
		}
	}
}

void randomize_chars(char** letters, int width, int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			letters[i][j] = (rand() % 2 == 0) ? 'F' : 'L';
		}
	}
}

Labyrinthe* generate_labyrinthe(int width, int height) {

	int i, stime;
	long ltime;
	ltime = time(NULL);
	stime = (unsigned)ltime / 2;

	Labyrinthe* labyrinth = malloc(sizeof(Labyrinthe));
	labyrinth->width = width;
	labyrinth->height = height;
	labyrinth->cells = malloc(sizeof(LabyrintheCell*) * width * height);

	for (int i = 0; i < width * height; i++) {
		labyrinth->cells[i] = malloc(sizeof(LabyrintheCell));
		labyrinth->cells[i]->type = PATH;
		for (int j = 0; j < 4; j++) {
			labyrinth->cells[i]->walls[j] = 0;
		}
	}

	labyrinth->cells[0]->type = START;
	labyrinth->cells[width * height - 1]->type = END;

	subdivise(labyrinth, 0, width, 0, height, 0);

	return labyrinth;
}

TTF_Font* font;

int hash_function(int x, int y) { return (x >> 16) ^ y ^ (x << 8); }

void afficher_labyrinthe(Labyrinthe* labyrinth, SDL_Renderer* renderer, Player* player,
				 char** letters) {

	/// Theme CPU
	// Clear l'écran en bleu
	SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
	SDL_RenderClear(renderer);

	static int iter = 0;

	const SDL_Color deco_col = {0, 30, 10, 255};
	const SDL_Color bg_col = {0, 0, 0, 255};
	const SDL_Color wall_green = {0, 100, 0, 255};
	const SDL_Color player_color = {255, 255, 255, 255};

	// Le background
	SDL_SetRenderDrawColor(renderer, bg_col.r, bg_col.g, bg_col.b, bg_col.a);
	SDL_Rect background = {0, 0, labyrinth->width * CELL_SIZE,
				     labyrinth->height * CELL_SIZE};
	SDL_RenderFillRect(renderer, &background);

	// On dessine le fond
	if (iter % 10 == 0) {
		randomize_chars(letters, labyrinth->width, labyrinth->height);
	}
	iter++;
	SDL_SetRenderDrawColor(renderer, deco_col.r, deco_col.g, deco_col.b, deco_col.a);
	for (int i = 0; i < labyrinth->width * labyrinth->height; i++) {
		int x = i % labyrinth->width;
		int y = i / labyrinth->width;
		// if (randint(0, 100) < 5) {
		char text[2];
		text[0] = letters[y][x];
		text[1] = '\0';
		SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, deco_col);
		SDL_Texture* text_texture =
		    SDL_CreateTextureFromSurface(renderer, text_surface);
		SDL_Rect text_rect = {x * CELL_SIZE + CELL_SIZE / 2 - text_surface->w / 2,
					    y * CELL_SIZE + CELL_SIZE / 2 - text_surface->h / 2,
					    text_surface->w, text_surface->h};
		SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);
		//}
	}

	// Les lignes
	SDL_SetRenderDrawColor(renderer, wall_green.r, wall_green.g, wall_green.b,
				     wall_green.a);
	for (int i = 0; i < labyrinth->width * labyrinth->height; i++) {
		int x = i % labyrinth->width;
		int y = i / labyrinth->width;

		if (labyrinth->cells[i]->walls[0]) {
			SDL_RenderDrawLine(renderer, x * CELL_SIZE, y * CELL_SIZE,
						 (x + 1) * CELL_SIZE, y * CELL_SIZE);
		}
		if (labyrinth->cells[i]->walls[1]) {
			SDL_RenderDrawLine(renderer, (x + 1) * CELL_SIZE, y * CELL_SIZE,
						 (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
		}
		if (labyrinth->cells[i]->walls[2]) {
			SDL_RenderDrawLine(renderer, x * CELL_SIZE, (y + 1) * CELL_SIZE,
						 (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
		}
		if (labyrinth->cells[i]->walls[3]) {
			SDL_RenderDrawLine(renderer, x * CELL_SIZE, y * CELL_SIZE,
						 x * CELL_SIZE, (y + 1) * CELL_SIZE);
		}
	}

	// On dessine des ronds qui bougent sur les lignes
	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	for (int i = 0; i < labyrinth->width * labyrinth->height; i++) {
		int x = i % labyrinth->width;
		int y = i / labyrinth->width;
		int rand = hash_function(x, y) % 5;
		if (rand != 0) {
			continue;
		}

		// on pose un carré un milieu de la ligne
		if (labyrinth->cells[i]->walls[0]) {
			int middle_x = x * CELL_SIZE + CELL_SIZE / 2 +
					   abs((iter % CELL_SIZE) - CELL_SIZE / 2);
			int middle_y = y * CELL_SIZE;
			SDL_Rect rect = {middle_x - 2, middle_y - 2, 4, 4};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[1]) {
			int middle_x = (x + 1) * CELL_SIZE;
			int middle_y = y * CELL_SIZE + CELL_SIZE / 2 +
					   abs((iter % CELL_SIZE) - CELL_SIZE / 2);
			SDL_Rect rect = {middle_x - 2, middle_y - 2, 4, 4};
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	// Le joueur
	{
		SDL_SetRenderDrawColor(renderer, player_color.r, player_color.g,
					     player_color.b, player_color.a);
		int quarter = CELL_SIZE / 4;
		SDL_Rect rect = {player->x * CELL_SIZE + (CELL_SIZE / 3),
				     player->y * CELL_SIZE + (CELL_SIZE / 3), quarter, quarter};
		SDL_RenderFillRect(renderer, &rect);
	}

	// Le départ
	{
		SDL_SetRenderDrawColor(renderer, 255, 205, 50, 255);
		int half = CELL_SIZE / 2;
		SDL_Rect rect = {half / 2, half / 2, half, half};
		SDL_RenderFillRect(renderer, &rect);
	}

	// L'arrivée (quadrillage noir et blanc)
	const SDL_Color black = {0, 0, 0, 255};
	const SDL_Color white = {255, 255, 255, 255};
	SDL_Color colors[2] = {black, white};
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				SDL_Color color = colors[(i + j) % 2];
				SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
							     color.a);
				int quarter = CELL_SIZE / 4;
				SDL_Rect rect = {
				    labyrinth->width * CELL_SIZE - quarter - i * quarter,
				    labyrinth->height * CELL_SIZE - quarter - j * quarter,
				    quarter, quarter};
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}

	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	int quarter = CELL_SIZE / 4;
	SDL_Rect rect = {player->x * CELL_SIZE + (CELL_SIZE / 3),
			     player->y * CELL_SIZE + (CELL_SIZE / 3), quarter, quarter};
	SDL_RenderFillRect(renderer, &rect);

	// Un petit glow (on re-render les lignes mais en rectangle et additif (2 layers))
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);

	const SDL_Color glow_green = {0, 255, 0, 60};
	SDL_SetRenderDrawColor(renderer, glow_green.r, glow_green.g, glow_green.b,
				     glow_green.a);
	for (int i = 0; i < labyrinth->width * labyrinth->height; i++) {
		int x = i % labyrinth->width;
		int y = i / labyrinth->width;

		// rect à peine plus gros que la ligne
		if (labyrinth->cells[i]->walls[0]) {
			SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, 2};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[1]) {
			SDL_Rect rect = {(x + 1) * CELL_SIZE - 2, y * CELL_SIZE, 2, CELL_SIZE};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[2]) {
			SDL_Rect rect = {x * CELL_SIZE, (y + 1) * CELL_SIZE - 2, CELL_SIZE, 2};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[3]) {
			SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, 2, CELL_SIZE};
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	const SDL_Color glow_yellow = {127, 127, 0, 60};
	SDL_SetRenderDrawColor(renderer, glow_yellow.r, glow_yellow.g, glow_yellow.b,
				     glow_yellow.a);
	for (int i = 0; i < labyrinth->width * labyrinth->height; i++) {
		int x = i % labyrinth->width;
		int y = i / labyrinth->width;
		// rect à peine plus gros que la ligne
		if (labyrinth->cells[i]->walls[0]) {
			SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE + 1, 3};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[1]) {
			SDL_Rect rect = {(x + 1) * CELL_SIZE - 3, y * CELL_SIZE, 3,
					     CELL_SIZE + 1};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[2]) {
			SDL_Rect rect = {x * CELL_SIZE, (y + 1) * CELL_SIZE - 3, CELL_SIZE + 1,
					     3};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (labyrinth->cells[i]->walls[3]) {
			SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, 3, CELL_SIZE + 1};
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

int can_move(Player* player, Labyrinthe* labyrinthe, int direction) {
	int x = player->x;
	int y = player->y;
	switch (direction) {
	case SDLK_UP:
		if (y == 0) {
			return 0;
		}
		return !labyrinthe->cells[(y - 1) * labyrinthe->width + x]->walls[2];
	case SDLK_DOWN:
		if (y == labyrinthe->height - 1) {
			return 0;
		}
		return !labyrinthe->cells[(y + 1) * labyrinthe->width + x]->walls[0];
	case SDLK_LEFT:
		if (x == 0) {
			return 0;
		}
		return !labyrinthe->cells[y * labyrinthe->width + x - 1]->walls[1];
	case SDLK_RIGHT:
		if (x == labyrinthe->width - 1) {
			return 0;
		}
		return !labyrinthe->cells[y * labyrinthe->width + x + 1]->walls[3];
	default:
		return 0;
	}
}

void move_window(SDL_Window* window) {
	// bouge la fenêtre en carré
	static int iter = 0;
	int pos_x, pos_y;
	const int SIZE = 10;
	const int SPEED = 3;
	if (iter % SPEED != 0) {
		goto fin;
	}
	SDL_GetWindowPosition(window, &pos_x, &pos_y);
	if (iter < SIZE) {
		SDL_SetWindowPosition(window, pos_x + 1, pos_y);
	}
	else if (iter < SIZE * 2) {
		SDL_SetWindowPosition(window, pos_x, pos_y + 1);
	}
	else if (iter < SIZE * 3) {
		SDL_SetWindowPosition(window, pos_x - 1, pos_y);
	}
	else if (iter < SIZE * 4) {
		SDL_SetWindowPosition(window, pos_x, pos_y - 1);
	}
	else {
		iter = 0;
	}
fin:
	iter++;
}

int has_won_labyrinthe(Player* player, Labyrinthe* labyrinthe) {
	return labyrinthe->cells[player->y * labyrinthe->width + player->x]->type == END;
}

int labyrinthe(int* timer) {

	font = TTF_OpenFont("fonts/Game_Of_Squids.ttf", 75);

	const int WIN_X = 1200;
	const int WIN_Y = 800;

	SDL_Window* window =
	    SDL_CreateWindow("Labyrinthe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

	intro("Labyrinthe", timer, renderer, window);

	int width = rand() % 20 + 20;
	int height = rand() % 20 + 20;

	Labyrinthe* labyrinth = generate_labyrinthe(width, height);
	int new_width = width * CELL_SIZE;
	int new_height = height * CELL_SIZE;
	SDL_SetWindowSize(window, new_width, new_height);
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	Player player = {
	    .x = 0,
	    .y = 0,
	};

	char** letters = malloc(sizeof(char*) * height);
	for (int i = 0; i < height; i++) {
		letters[i] = malloc(sizeof(char) * width);
	}
	randomize_chars(letters, width, height);

	SDL_Event event;
	int success = 0;
	while ((!success) && (*timer > 0)) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_UP) {
					if (can_move(&player, labyrinth, SDLK_UP)) {
						player.y--;
					}
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					if (can_move(&player, labyrinth, SDLK_DOWN)) {
						player.y++;
					}
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					if (can_move(&player, labyrinth, SDLK_LEFT)) {
						player.x--;
					}
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					if (can_move(&player, labyrinth, SDLK_RIGHT)) {
						player.x++;
					}
				}
			}
			success = has_won_labyrinthe(&player, labyrinth);
		}
		// Le rendu
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		// SDL_RenderPresent(renderer);
		//  Render labyrinth
		afficher_labyrinthe(labyrinth, renderer, &player, letters);
		SDL_RenderPresent(renderer);
		move_window(window);

		// printf("%d\n", *timer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return success;
}