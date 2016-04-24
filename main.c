#include <stdio.h>
#include "isa.h"
#include "debug.h"
#include "execute.h"
#include "graphics.h"
#include "types.h"
extern char show_parser_output;

int main(int argc, char **argv) {
	SET_LOW(REG(AX),255);
	SET_HI(REG(AX),12);
    show_parser_output = 1;
    if (argc <= 1) {
        init(500,500);
	    printf("Welcome to our 8086 Emulator...\n");
	    run_sdl_test();
    } else if (argc == 2) {
        exec_prog(argv[1]);
    } else if (argc == 3) {
        if (strlen(argv[2]) == 1 && (argv[2][0] == '0' || argv[2][0] == '1')) {
            show_parser_output = argv[2][0] - '0';
            exec_prog(argv[1]);
        } else {
            printf("usage: ./8086emulator prog-name [0-1]\n");
        }
    } else {
        printf("usage: ./8086emulator prog-name [0-1]\n");
    }
    //exec_prog("hex-bin.com");
	//exec_prog("pali.com");
    //exec_prog("snake.com");
	//print_regs();
}
