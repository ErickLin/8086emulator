#include "isa.h"
#include <stdio.h>

void SET_FLAG(u32 i){ FLAGS|=(0x1 << i);}
u32 GET_FLAG(u32 i){ return ( FLAGS & (0x1 << i));}
void CLR_FLAG(u32 i){ FLAGS&=~(0x1 << i);};
void SET_HI(u16 *reg,u8 data){ *reg= (*reg & 0x00ff) | (data << 8); }
u16 GET_HI(u16 reg){return ((reg & 0xff00) >> 8);}
void SET_LOW(u16 *reg,u8 data){ *reg = (*reg & 0xff00) | data;}
u16 GET_LOW(u16 reg){return (reg & 0x00ff);}
void CLR_NIB(u16* reg,u16 index){ *reg = (*reg & ~(0x000f << 4*index));}
u8 PARITY(s16 v){
	u8 parity=0;
	while(v){ parity=!parity; v = v & (v-1);}
	return parity;
}

u8 OVERFLOW_SUM(u16 a,u16 b){
	if((SIGN(a) && SIGN(b)) || (!SIGN(a) && !SIGN(b))){
		if(SIGN(a) && !(SIGN(a+b))){ return 1;}
	}
	return 0;
}




//ISA implementation

void AAA(){
	if(NIBBLE(AX) > 9 || GET_FLAG(AF)){
		SET_LOW(&AX,GET_LOW(AX)+6);
		SET_HI(&AX,GET_HI(AX)+1);
		SET_FLAG(AF);
		SET_FLAG(CF);
	}else{
		CLR_FLAG(AF);
		CLR_FLAG(CF);
	}
	CLR_NIB(&AX,1);
}

void AAD(){
	SET_LOW(&AX,GET_HI(AX)*10+GET_LOW(AX));
	SET_HI(&AX,0);
	(!AX) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(AX)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(AX)) ? CLR_FLAG(PF) : SET_FLAG(PF);
}

void AAM(){
	SET_HI(&AX,GET_LOW(AX)/10);
	SET_LOW(&AX,GET_LOW(AX)%10);
	(!AX) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(AX)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(AX)) ? CLR_FLAG(PF) : SET_FLAG(PF);
}

void AAS(){
	if(NIBBLE(AX) > 9 || GET_FLAG(AF)){
		SET_LOW(&AX,GET_LOW(AX)-6);
		SET_HI(&AX,GET_HI(AX)-1);
		SET_FLAG(AF);
		SET_FLAG(CF);
	}else{
		SET_FLAG(AF);
		SET_FLAG(CF);
	}
	CLR_NIB(&AX,1);
}

//Uses the effective address for memory references

void ADC_Reg_Mem(s16* reg,u32 eff_add){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + MEM[eff_add] + carry;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,MEM[eff_add]);
	if(!overflow) overflow=OVERFLOW_SUM(*reg+MEM[eff_add],carry);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? SET_FLAG(PF) : CLR_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg = sum;
}

void ADC_Mem_Reg(u32 eff_add,s16* reg){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + MEM[eff_add] + carry;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,MEM[eff_add]);
	if(!overflow) overflow=OVERFLOW_SUM(*reg+MEM[eff_add],carry);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? SET_FLAG(PF) : CLR_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	MEM[eff_add]=sum;
}

void ADC_Reg_Reg(s16* reg_a,s16* reg_b){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg_a + *reg_b + carry;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg_a,*reg_b);
	if(!overflow) overflow=OVERFLOW_SUM(*reg_a+*reg_b,carry);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? SET_FLAG(PF) : CLR_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg_a=sum;
}

void ADC_Mem_Imm(u32 eff_add,s16 imm){
	u32 carry=GET_FLAG(CF);
	u32 sum= MEM[eff_add] + imm + carry;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(MEM[eff_add],imm);
	if(!overflow) overflow=OVERFLOW_SUM(MEM[eff_add]+imm,carry);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? SET_FLAG(PF) : CLR_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	MEM[eff_add]=sum;
}

void ADC_Reg_Imm(s16* reg,s16 imm){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + imm + carry;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,imm);
	if(!overflow) overflow=OVERFLOW_SUM(*reg+imm,carry);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? SET_FLAG(PF) : CLR_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg = sum;
}

