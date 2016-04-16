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

