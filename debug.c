#include <stdio.h>
#include "debug.h"
#include "isa.h"

void print_mem(){
	u32 i;
	for(i=0;i<MEMSIZE;i++){
		printf("0x%x : 0x%x\n",i,MEM[i]);
	}
}

void print_mem_rg(u32 start,u32 end){
	u32 i;
	for(i=start;i<end;i++){
		printf("0x%x : 0x%x\n",i,MEM[i]);
	}
}

void print_regs(){
	printf("General Purpose:\n");
	printf("AX: 0x%04hx\nBX: 0x%04hx\nCX: 0x%04hx\nDX: 0x%04hx\nSI: 0x%04hx\nDI: 0x%04hx\nBP: 0x%04hx\nSP: 0x%04hx\n",AX,BX,CX,DX,SI,DI,BP,SP_);
	printf("Segment Registers:\n");
	printf("CS: 0x%04hx\nDS: 0x%04hx\nES: 0x%04hx\nSS: 0x%04hx\n",CS,DS,ES,SS);
	printf("Special Purpose:\n");
	printf("IP: 0x%04hx\nFLAGS: 0x%04hx\n",IP,FLAGS);
}

void print_flags(){
	printf("FLAGS: \n");
	int i;
	printf("X |X |X |X |OF|DF|IF|TF|SF|ZF|X |AF|X |PF|X |CF|\n");
	for(i=15;i>=0;i--){
		printf("%d |",((BITMASK(i) & FLAGS)) ? 1 : 0);
	}
	printf("\n");
}


