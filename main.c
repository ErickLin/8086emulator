#include <stdio.h>
#include "isa.h"
#include "debug.h"
#include "graphics.h"
#include "types.h"

int main(int argc,char **argv){
	printf("Welcome to our 8086 Emulator...\n");
	init(500,500);
	SET_LOW(REG(AX),255);
	SET_HI(REG(AX),12);
	for(int i=0;i<250;i++){
		*REG(CX)=i;
		*REG(DX)=i;
		INT10();
		SDL_Flip(screen);
	}
	//set_pixel(100,100,255);
	

	print_regs();
	run();
}
