#include "../intro.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct fuze_t {
	int status;
	int hardness;
	int power;
} fuze;

Uint32 switch_callback(Uint32 interval, void* param) {
	if (((fuze*)(((void**)param)[0]))->hardness > rand() % 100) {
		((fuze*)(((void**)param)[0]))->status = 0;
		*((int*)(((void**)param)[2])) -= ((fuze*)(((void**)param)[0]))->power;
		SDL_RemoveTimer(*((SDL_TimerID*)(((void**)param)[1])));
		*(((void***)param)[1]) = NULL;
		free(param);
		return 0;
	}
	return interval;
}

void switch_status(fuze* fuze, SDL_TimerID* timer_slot, int total_power, int* power) {
	if (fuze->status == 2) {
		return;
	}
	if (fuze->status == 1) {
		fuze->status = 0;
		*power -= fuze->power;
		return;
	}
	if (fuze->hardness > rand() % 30 || *power + fuze->power >= total_power) {
		fuze->status = 2;
		*power += fuze->power;
		void** params = (void**)malloc(sizeof(void*) * 3);
		params[0] = fuze;
		params[1] = timer_slot;
		params[2] = power;
		*timer_slot = SDL_AddTimer(50, switch_callback, params);
	}
	else {
		fuze->status = 1;
		*power += fuze->power;
	}
}

/*int disjoncteur(int* timer,const int temps){
    int timer_end=*timer-temps;
    srand((time)NULL);
    //fuzes
    int nb_fuze = 10;
    fuze l_fuze[nb_fuze];
    int pc = rand()%nb_fuze;
    int box=pc;
    while(pc==box) box=rand()%nb_fuze;
    int heater=pc;
    while(pc==heater) heater=rand()%nb_fuze;
    for(int i=0;i<nb_fuze;i++){
	  l_fuze[i].status=0;
	  l_fuze[i].hardness=rand()%75;
	  l_fuze[i].power=rand()%100+100;
    }
    int total_power = 500;
    int power =0;
    l_fuze[heater].power = rand()%300+200;

    //init window
    SDL_Window*screen=SDL_CreateWindow("Disjoncteur", SDL_WINDOWPOS_CENTERED,
SDL_WINDOWPOS_CENTERED, 1800,900,SDL_WINDOW_SHOWN); SDL_Renderer*renderer =
SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);


    //image load
    SDL_Surface*image_surface = IMG_Load("./minijeux/disjoncteur/switch_off.png");
    SDL_Texture*switch_off_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);
    image_surface = IMG_Load("./minijeux/disjoncteur/switch_on.png");
    SDL_Texture*switch_on_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);
    image_surface = IMG_Load("./minijeux/disjoncteur/switch_mid.png");
    SDL_Texture*switch_mid_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);
    image_surface = IMG_Load("./minijeux/disjoncteur/box.png");
    SDL_Texture*box_off_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);
    image_surface = IMG_Load("./minijeux/disjoncteur/box_on.png");
    SDL_Texture*box_on_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);

    image_surface = IMG_Load("./minijeux/disjoncteur/pc_reveille.png");
    SDL_Texture*pc_on_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);

    image_surface = IMG_Load("./minijeux/disjoncteur/pc_endormie.png");
    SDL_Texture*pc_off_texture = SDL_CreateTextureFromSurface(renderer,image_surface);
    SDL_FreeSurface(image_surface);


    SDL_TimerID l_timer[10];

    //boucle
    int done=1000;
    SDL_Event event;
    while(*timer>=timer_end && done){
	  if (l_fuze[pc].status==1 && l_fuze[box].status==1) done--;
	  else done=1000;

	  //touche
	  while(SDL_PollEvent(&event)){
		switch(event.type){
		    case SDL_QUIT:
			  exit(1);
		    case SDL_KEYDOWN:
			  if(event.key.keysym.sym==SDLK_ESCAPE) exit(1);
			  break;
		    case SDL_MOUSEBUTTONDOWN:
			  //test si la souris est sur un interupteur
			  if(event.button.y>=700 && event.button.y<=770){
				int j=(event.button.x-100);
				if (j/120>=0 && j/120<=nb_fuze && j%120<=100){
				    switch_status(&(l_fuze[j/120]),&(l_timer[j/120]),total_power,&power);
				}
			  }
			  break;
		}
	  }

	  //affichage
	  SDL_SetRenderDrawColor(renderer, 255, 236, 206, 255);
	  SDL_RenderClear(renderer);
	  SDL_Rect rect;
	  rect.w=100;
	  rect.h=70;
	  rect.y=700;
	  for(int i=0;i<nb_fuze;i++){
		rect.x = 100+120*i;
		SDL_RenderCopy(renderer,
(l_fuze[i].status)?((l_fuze[i].status==1)?switch_on_texture:switch_mid_texture):switch_off_texture,NULL,&rect);
	  }
	  //computer
	  rect.w=300;
	  rect.h=300;
	  rect.y=200;
	  rect.x=200;
	if(l_fuze[pc].status==1) SDL_RenderCopy(renderer, pc_on_texture,NULL,&rect);
	  else SDL_RenderCopy(renderer, pc_off_texture,NULL,&rect);
	//box
	  rect.w=200;
	  rect.h=200;
	  rect.x=500;
	  rect.y=210;
	  if(l_fuze[box].status==1) SDL_RenderCopy(renderer, box_on_texture,NULL,&rect);
	  else SDL_RenderCopy(renderer, box_off_texture,NULL,&rect);
	  SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(switch_mid_texture);
    SDL_DestroyTexture(switch_off_texture);
    SDL_DestroyTexture(switch_on_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    return (done==0);
}*/

int disjoncteur(int* timer) {
	srand(time(NULL));
	// fuzes
	int nb_fuze = 10;
	fuze l_fuze[nb_fuze];
	int pc = rand() % nb_fuze;
	int box = pc;
	while (pc == box) {
		box = rand() % nb_fuze;
	}
	int heater = pc;
	while (pc == heater) {
		heater = rand() % nb_fuze;
	}
	for (int i = 0; i < nb_fuze; i++) {
		l_fuze[i].status = 0;
		l_fuze[i].hardness = rand() % 15;
		l_fuze[i].power = rand() % 100 + 100;
	}
	int total_power = 3000;
	int power = 0;
	l_fuze[heater].power = rand() % 300 + 200;

	// init window
	SDL_Window* screen =
	    SDL_CreateWindow("Disjoncteur", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				   1300, 800, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

	// image load
	SDL_Surface* image_surface = IMG_Load("fichiers/disjoncteur/switch_off.png");
	SDL_Texture* switch_off_texture =
	    SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);
	image_surface = IMG_Load("fichiers/disjoncteur/switch_on.png");
	SDL_Texture* switch_on_texture =
	    SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);
	image_surface = IMG_Load("fichiers/disjoncteur/switch_mid.png");
	SDL_Texture* switch_mid_texture =
	    SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);
	image_surface = IMG_Load("fichiers/disjoncteur/box.png");
	SDL_Texture* box_off_texture =
	    SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);
	image_surface = IMG_Load("fichiers/disjoncteur/box_on.png");
	SDL_Texture* box_on_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);

	image_surface = IMG_Load("fichiers/disjoncteur/pc_reveille.png");
	SDL_Texture* pc_on_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);

	image_surface = IMG_Load("fichiers/disjoncteur/pc_endormi.png");
	SDL_Texture* pc_off_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
	SDL_FreeSurface(image_surface);

	intro("fusible", timer, renderer, screen);

	SDL_TimerID l_timer[10];

	// boucle
	int done = 0;
	SDL_Event event;
	while ((*timer > 0) && !done) {
		if (l_fuze[pc].status == 1 && l_fuze[box].status == 1) {
			done = 1;
		}

		// touche
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			// case SDL_QUIT:
			// exit(1);
			// case SDL_KEYDOWN:
			// if(event.key.keysym.sym==SDLK_ESCAPE) exit(1);
			// break;
			case SDL_MOUSEBUTTONDOWN:
				// test si la souris est sur un interupteur
				if (event.button.y >= 700 && event.button.y <= 770) {
					int j = (event.button.x - 100);
					if (j / 120 >= 0 && j / 120 <= nb_fuze && j % 120 <= 100) {
						switch_status(&(l_fuze[j / 120]), &(l_timer[j / 120]),
								  total_power, &power);
					}
				}
				break;
			}
		}

		// affichage
		SDL_SetRenderDrawColor(renderer, 255, 236, 206, 255);
		SDL_RenderClear(renderer);
		SDL_Rect rect;
		rect.w = 100;
		rect.h = 70;
		rect.y = 700;
		for (int i = 0; i < nb_fuze; i++) {
			rect.x = 100 + 120 * i;
			SDL_RenderCopy(renderer,
					   (l_fuze[i].status)
						 ? ((l_fuze[i].status == 1) ? switch_on_texture
										    : switch_mid_texture)
						 : switch_off_texture,
					   NULL, &rect);
		}
		// computer
		rect.w = 300;
		rect.h = 300;
		rect.y = 200;
		rect.x = 200;
		if (l_fuze[pc].status == 1) {
			SDL_RenderCopy(renderer, pc_on_texture, NULL, &rect);
		}
		else {
			SDL_RenderCopy(renderer, pc_off_texture, NULL, &rect);
		}
		// box
		rect.w = 200;
		rect.h = 200;
		rect.x = 500;
		rect.y = 210;
		if (l_fuze[box].status == 1) {
			SDL_RenderCopy(renderer, box_on_texture, NULL, &rect);
		}
		else {
			SDL_RenderCopy(renderer, box_off_texture, NULL, &rect);
		}
		SDL_RenderPresent(renderer);
	}
	SDL_Delay(1000);
	SDL_DestroyTexture(switch_mid_texture);
	SDL_DestroyTexture(switch_off_texture);
	SDL_DestroyTexture(switch_on_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(screen);
	return done;
}
