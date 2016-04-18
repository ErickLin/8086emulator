#include <stdio.h>
//#include "isa.h"
//#include "debug.h"
#include "graphics.h"

int main(int argc,char **argv){
	//printf("Welcome to our 8086 Emulator...\n");

	init(500,500);
	u32 data=0xffffffff;
	for(int i=0;i<WIDTH;i++){
		set_pixel(i,HEIGHT/2,data);
	}
	SDL_Flip(screen);
	run();
}
