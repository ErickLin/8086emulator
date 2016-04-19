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
	//exec("DOSDEF.COM");
	//exec("hello.com");
	//exec("test.out");
	//exec("add-sub.com");
	//exec("calc-sum.com");
	//exec("hex-bin.com");
	//exec("hi-world.com");
	//exec("pali.com");
    //exec("viksiege.exe");
    //exec("snake.com");
	run();
	//print_regs();
}
