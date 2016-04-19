#include <stdio.h>
#include "isa.h"
#include "debug.h"
#include "graphics.h"
#include "types.h"
#include "execute.h"

int main(int argc,char **argv){
	printf("Welcome to our 8086 Emulator...\n");
	
	//exec("DOSDEF.COM");
	//exec("hello.com");
	//exec("test.out");
	//exec("add-sub.com");
	//exec("calc-sum.com");
	
	//exec("hi-world.com");
	//exec("pali.com");
    //exec("viksiege.exe");
    //exec("snake.com");



    init(500,500);
	SET_LOW(REG(AX),255);
	SET_HI(REG(AX),12);
	run();
    //exec_com("hex-bin.com");
	
	//print_regs();
}
