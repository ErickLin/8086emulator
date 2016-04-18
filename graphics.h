#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "types.h"
#include "SDL/SDL.h"

SDL_Surface *screen;
u16 WIDTH;
u16 HEIGHT;

u32 init(u16,u16);
void run(); //TODO?
u32 get_pixel(u16,u16);
void set_pixel(u16,u16,u32);


#endif