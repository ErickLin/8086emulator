#include "graphics.h"
#include "isa.h"

void update();

void render();

u32 init(u16 w,u16 h){
	SDL_Init(SDL_INIT_EVERYTHING);
	WIDTH=w;
	HEIGHT=h;
	screen=SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_SWSURFACE);
	SDL_Flip(screen);
}

void run_test(){
	done=0;
	while(!done){
		while(SDL_PollEvent(&event)){

			
			//render();

			if(event.type == SDL_QUIT){
				done=1;
			}

			if(event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
					case SDLK_RIGHT: dir=0; break;
					case SDLK_LEFT: dir=1; break;
					case SDLK_DOWN: dir=2; break;
					case SDLK_UP: dir=3; break;
				}
			}

		}
			SDL_Delay(50);
			update();
			render();
			//set_pixel(xpos,ypos,255);
			//SDL_Flip(screen);
	}
}

u32 get_pixel(u16 x,u16 y){
	return *((u32*)screen->pixels);
}

void set_pixel(u16 x,u16 y,u32 col){
	*(((u32*)screen->pixels)+WIDTH*y+x)=col;
}


void update(){
	switch(dir){
		case 0:
			xpos++;
			break;
		case 1:
			xpos--;
			break;
		case 2:
			ypos++;
			break;
		case 3:
			ypos--;
			break;
	}
	if(xpos > WIDTH) xpos=0;
	else if(xpos < 0) xpos=WIDTH;
	else if(ypos > HEIGHT) ypos=0;
	else if(ypos < 0) ypos=WIDTH;
	*REG(CX)=xpos;
	*REG(DX)=ypos;
}

void render(){
		//
		//get_pixel(0,0);
		set_pixel(*REG(CX),*REG(DX),255);
		//INT(0x10);
		SDL_Flip(screen);
}
