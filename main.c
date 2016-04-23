#include <stdio.h>
#include "isa.h"
#include "debug.h"
#include "execute.h"
#include "graphics.h"
#include "types.h"

int main(int argc,char **argv){
	printf("Welcome to our 8086 Emulator...\n");
    init(500,500);
	SET_LOW(REG(AX),255);
	SET_HI(REG(AX),12);
	//run_test();
	
	//exec_prog("hello.com");
	//exec_prog("test.out");
	//exec_prog("add-sub.com");
	//exec_prog("calc-sum.com");
    //exec_prog("hex-bin.com");
	//exec_prog("hi-world.com");
	//exec_prog("pali.com");
    //exec_prog("viksiege.exe");
    exec_prog("snake.com");
	
	//print_regs();
}
