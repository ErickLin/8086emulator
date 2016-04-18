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
	for(int i=AX;i<CS;i++){
		printf("%c%c: 0x%04hx\n",*REG_STR[i],*(REG_STR[i]+1),*REG(i));
	}
	printf("Segment Registers:\n");
	for(int i=CS;i<IP;i++){
		printf("%c%c: 0x%04hx\n",*REG_STR[i],*(REG_STR[i]+1),*REG(i));
	}
	printf("Special Purpose:\n");
	for(int i=IP;i<=FLAGS;i++){
		printf("%c%c: 0x%04hx\n",*REG_STR[i],*(REG_STR[i]+1),*REG(i));
	}
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


