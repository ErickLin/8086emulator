#include "isa.h"
#include <stdio.h>

void SET_FLAG(u32 i){ FLAGS|=(0x1 << i);}
u32 GET_FLAG(u32 i){ return ( FLAGS & (0x1 << i));}
void TOGGLE_FLAG(u32 i){ (GET_FLAG(i)) ? CLR_FLAG(i) : SET_FLAG(i);}
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

//ADC

void ADC_Reg_Mem(s16* reg,u32 eff_add){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + MEM[eff_add] + carry;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,MEM[eff_add]);
	if(!overflow) overflow=OVERFLOW_SUM(*reg+MEM[eff_add],carry);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
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
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
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
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
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
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
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
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg = sum;
}

//ADD

void ADD_Reg_Mem(s16* reg,u32 eff_add){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + MEM[eff_add];
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,MEM[eff_add]);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg = sum;
}

void ADD_Mem_Reg(u32 eff_add,s16* reg){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + MEM[eff_add];
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,MEM[eff_add]);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	MEM[eff_add]=sum;
}


void ADD_Reg_Reg(s16* reg_a,s16* reg_b){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg_a + *reg_b;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg_a,*reg_b);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg_a=sum;
}


void ADD_Mem_Imm(u32 eff_add,s16 imm){
	u32 carry=GET_FLAG(CF);
	u32 sum= MEM[eff_add] + imm;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(MEM[eff_add],imm);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	MEM[eff_add]=sum;
}

void ADD_Reg_Imm(s16* reg,s16 imm){
	u32 carry=GET_FLAG(CF);
	u32 sum= *reg + imm;
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	u32 overflow=OVERFLOW_SUM(*reg,imm);
	(overflow) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
	*reg = sum;
}

//AND

void AND_Reg_Mem(s16* reg,u32 eff_add){
	u16 res=*reg & MEM[eff_add];
	(!res) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(res)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(res)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	*reg=res;
}
void AND_Mem_Reg(u32 eff_add,s16* reg){
	u16 res=MEM[eff_add] & *reg;
	(!res) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(res)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(res)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	MEM[eff_add]=res;
}
void AND_Reg_Reg(s16* reg_a,s16* reg_b){
	u16 res=*reg_a & *reg_b;
	(!res) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(res)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(res)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	*reg_a=res;
}
void AND_Mem_Imm(u32 eff_add,s16 imm){
	u16 res=MEM[eff_add] & imm;
	(!res) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(res)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(res)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	MEM[eff_add]=res;
}
void AND_Reg_Imm(s16* reg,s16 imm){
	u16 res=*reg & imm;
	(!res) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(res)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(res)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	*reg=res;
}

void CALL(u32 eff_add){
	MEM[SP]=IP;
	SP+=4;
	MEM[SP]=CS;
	SP+=4;
	IP=eff_add;
}


void CBW(){
	if((0x80) & AX) SET_HI(&AX,0x00ff);
	else SET_HI(&AX,0);
}

void CLC(){CLR_FLAG(CF);}

void CLI(){CLR_FLAG(IF);}

void CMC(){
	if(GET_FLAG(CF)) CLR_FLAG(CF);
	else SET_FLAG(CF);
}

void CMP_Reg_Mem(s16* reg,u32 eff_add){
	ADD_Reg_Imm(reg,TC((s16)MEM[eff_add]));
}
void CMP_Mem_Reg(u32 eff_add,s16* reg){
	ADD_Mem_Imm(eff_add,TC((s16)*reg));
}
void CMP_Reg_Reg(s16* reg_a,s16* reg_b){
	ADD_Reg_Imm(reg_a,TC((s16)*reg_b));
}
void CMP_Mem_Imm(u32 eff_add,s16 imm){
	ADD_Mem_Imm(eff_add,TC((s16)imm));
}
void CMP_Reg_Imm(s16* reg,s16 imm){
	ADD_Reg_Imm(reg,TC((s16)imm));
}

void CMPSB(){
	u16 a=MEM[DS+SI];
	u16 b=MEM[ES+DI];
	u32 sum=a + TC(b);
	if(!GET_FLAG(DF)){
		SI++;
		DI++;
	}else{
		SI--;
		DI--;
	}
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(OVERFLOW_SUM(a,TC(b))) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
}

void CMPSW(){
	u16 a=MEM[DS+SI];
	u16 b=MEM[ES+DI];
	u32 sum=a + TC(b);
	if(!GET_FLAG(DF)){
		SI+=2;
		DI+=2;
	}else{
		SI-=2;
		DI-=2;
	}
	(CARRY(sum)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!sum) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(sum)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(OVERFLOW_SUM(a,TC(b))) ? SET_FLAG(OF) : CLR_FLAG(OF);
	(PARITY(sum)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(sum)) ? SET_FLAG(AF) : CLR_FLAG(AF);
}

void CWD(){
	if(SIGN(AX)) DX=0xffff;
	else DX=0;
}

void DAA(){
	if((NIBBLE(AX) > 9) || GET_FLAG(AF)){
		SET_LOW(&AX,GET_LOW(AX)+6);
		(OVERFLOW_SUM(GET_LOW(AX),6)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_FLAG(AF);
	}
	if((NIBBLE(AX) > 0x9f) || GET_FLAG(CF)){
		SET_LOW(&AX,GET_LOW(AX)+0x60);
		(OVERFLOW_SUM(GET_LOW(AX),0x60)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_FLAG(CF);
	}
	(CARRY(AX)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!AX) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(AX)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(AX)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(AX)) ? SET_FLAG(AF) : CLR_FLAG(AF);

}

void DAS(){
	if((NIBBLE(AX) > 9) || GET_FLAG(AF)){
		SET_LOW(&AX,GET_LOW(AX)+TC(6));
		(OVERFLOW_SUM(GET_LOW(AX),TC(6))) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_FLAG(AF);
	}
	if((NIBBLE(AX) > 0x9f) || GET_FLAG(CF)){
		SET_LOW(&AX,GET_LOW(AX)+TC(0x60));
		(OVERFLOW_SUM(GET_LOW(AX),TC(0x60))) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_FLAG(CF);
	}
	(CARRY(AX)) ? SET_FLAG(CF) : CLR_FLAG(CF);
	(!AX) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(AX)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(AX)) ? CLR_FLAG(PF) : SET_FLAG(PF);
	(ADJUST(AX)) ? SET_FLAG(AF) : CLR_FLAG(AF);
}

void DEC_Reg(s16* reg){
	u16 carry=GET_FLAG(CF);
	ADD_Reg_Imm(reg,(s16)(*reg + TC(1)));
	if(GET_FLAG(CF) != carry){ TOGGLE_FLAG(CF);}
}
void DEC_Mem(u32 eff_add){
	u16 carry=GET_FLAG(CF);
	ADD_Mem_Imm(eff_add,(s16)(MEM[eff_add] + TC(1)));
	if(GET_FLAG(CF) != carry){ TOGGLE_FLAG(CF);}
}

void DIV_Reg_8b(s16* reg){
	SET_LOW(&AX,AX/((u16)*reg));
	SET_HI(&AX,AX%((u16)*reg));
}
void DIV_Mem_8b(u32 eff_add){
	SET_LOW(&AX,AX/((u16)MEM[eff_add]));
	SET_HI(&AX,AX%((u16)MEM[eff_add]));
}
void DIV_Reg_16b(s16* reg){
	u32 merge=(DX << 16) | AX;
	AX=merge/((u16)*reg);
	DX=merge%((u16)*reg);
}
void DIV_Mem_16b(u32 eff_add){
	u32 merge=(DX << 16) | AX;
	AX=merge/((u16)MEM[eff_add]);
	DX=merge%((u16)MEM[eff_add]);
}

void IDIV_Reg_8b(s16* reg){
	SET_LOW(&AX,AX/(*reg));
	SET_HI(&AX,AX%(*reg));
}
void IDIV_Mem_8b(u32 eff_add){
	SET_LOW(&AX,AX/(MEM[eff_add]));
	SET_HI(&AX,AX%(MEM[eff_add]));
}
void IDIV_Reg_16b(s16* reg){
	u32 merge=(DX << 16) | AX;
	AX=merge/(*reg);
	DX=merge%(*reg);
}
void IDIV_Mem_16b(u32 eff_add){
	u32 merge=(DX << 16) | AX;
	AX=merge/(MEM[eff_add]);
	DX=merge%(MEM[eff_add]);
}

void IMUL_Reg_8b(s16* reg){
	s32 temp=GET_LOW(AX)*(*reg);
	//NEED to set flags too daniel you're drunk go to bed
	AX=temp;
}
void IMUL_Mem_8b(u32 eff_add){
	AX=GET_LOW(AX)*(MEM[eff_add]);
}
void IMUL_Reg_16b(s16 * reg){
	u32 temp = AX*(*reg);
	DX=(temp >> 0xf) & 0x00ff;
	AX=temp;
}
void IMUL_Mem_16b(u32 eff_add); 

void INC_Reg(s16* reg){
	u16 carry=GET_FLAG(CF);
	ADD_Reg_Imm(reg,(s16)(*reg + 1));
	if(GET_FLAG(CF) != carry){ TOGGLE_FLAG(CF);}
}
void INC_Mem(u32 eff_add){
	u16 carry=GET_FLAG(CF);
	ADD_Mem_Imm(eff_add,(s16)(MEM[eff_add] + 1));
	if(GET_FLAG(CF) != carry){ TOGGLE_FLAG(CF);}
}

void INT(u32 i){

}

void INTO(){
	if(GET_FLAG(OF)) INT(4);
}


