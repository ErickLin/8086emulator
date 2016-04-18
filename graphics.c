#include "graphics.h"

u32 init(u16 w,u16 h){
	SDL_Init(SDL_INIT_EVERYTHING);
	WIDTH=w;
	HEIGHT=h;
	screen=SDL_SetVideoMode(WIDTH,HEIGHT,32,SDL_SWSURFACE);
	SDL_Flip(screen);
}

void run(){
	SDL_Delay(2000);
}

u32 get_pixel(u16 x,u16 y){
	return *((u32*)screen->pixels);
}

void set_pixel(u16 x,u16 y,u32 col){
	*(((u32*)screen->pixels)+WIDTH*y+x)=col;
}
