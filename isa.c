#include "isa.h"
#include <stdio.h>

reg_file_t reg_file;

const char *REG_STR[]={"AX\0","BX\0","CX\0","DX\0","SI\0","DI\0","BP\0","SP\0","CS\0","DS\0","ES\0","SS\0","IP\0","FL\0"};


void SET_FLAG(u32 i){ *REG(FLAGS) |=(0x1 << i);}
u32 GET_FLAG(u32 i){ return ( *REG(FLAGS) & (0x1 << i));}
void TOGGLE_FLAG(u32 i){ (GET_FLAG(i)) ? CLR_FLAG(i) : SET_FLAG(i);}
void CLR_FLAG(u32 i){ *REG(FLAGS) &=~(0x1 << i);}

void SET_HI(s16 *reg,u8 data){ *reg= (*reg & 0x00ff) | (data << 8); }
u16 GET_HI(s16 reg){return ((reg & 0xff00) >> 8);}
void SET_LOW(s16 *reg,u8 data){ *reg = (*reg & 0xff00) | data;}
u16 GET_LOW(s16 reg){return (reg & 0x00ff);}
void CLR_NIB(s16* reg,u16 index){ *reg = (*reg & ~(0x000f << 4*index));}
s16* REG(u8 entry){ return (s16*)&reg_file + entry;}

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

void FLAG_CHECK(u32 data,u8 c,u8 z,u8 s,u8 p,u8 a){
	if(c) {(CARRY(data)) ? SET_FLAG(CF) : CLR_FLAG(CF);}
	if(z){(!data) ? SET_FLAG(ZF) : CLR_FLAG(ZF);}
	if(s){(SIGN(data)) ? SET_FLAG(SF) : CLR_FLAG(SF);}
	//Overflow not here
    if(p){(PARITY(data)) ? SET_FLAG(PF) : CLR_FLAG(PF);}
    if(a){ADJUST(data) ? SET_FLAG(AF) : CLR_FLAG(AF);}
}

u32 MMM(u8 mmm){
	switch(mmm){
		case 0:
			return ABS(DS,BX+SI);
			break;
		case 1:
			return ABS(DS,BX+DI);
			break;
		case 2:
			return ABS(SS,BP+SI);
			break;
		case 3:
			return ABS(SS,BP+DI);
			break;
		case 4:
			return ABS(DS,SI);
			break;
		case 5:
			return ABS(DS,DI);
			break;
		case 6:
			return ABS(SS,BP);
			break;
		case 7:
			return ABS(DS,BX);
			break;
	}	
}

u8 RRR(u8 rrr){
	switch(rrr){
		case 0:
			return AX;
			break;
		case 1:
			return CX;
			break;
		case 2:
			return DX;
			break;
		case 3:
			return BX;
			break;
		case 5:
			return BP;
			break;
		case 6:
			return SI;
			break;
		case 7:
			return DI;
			break;
	}	
}




//ISA implementation
void AAA(){
	s16* ax=REG(AX);
	if(NIBBLE(*ax) > 9 || GET_FLAG(AF)){
		SET_LOW(ax,GET_LOW(*ax)+6);
		SET_HI(ax,GET_HI(*ax)+1);
		SET_FLAG(AF);
		SET_FLAG(CF);
	}else{
		CLR_FLAG(AF);
		CLR_FLAG(CF);
	}
	CLR_NIB(ax,1);
}

void AAD(){
	s16* ax=REG(AX);
	SET_LOW(ax,GET_HI(*ax)*10+1);
	SET_HI(ax,0);
	FLAG_CHECK(*ax,0,1,1,1,0);
}

void AAM(){
	s16 *ax=REG(AX);
	SET_HI(ax,GET_LOW(*ax)/10);
	SET_LOW(ax,GET_LOW(*ax)%10);
	FLAG_CHECK(*ax,0,1,1,1,0);
}

void AAS(){
	s16* ax=REG(AX);
	if(NIBBLE(*ax) > 9 || GET_FLAG(AF)){
		SET_LOW(ax,GET_LOW(*ax)-6);
		SET_HI(ax,GET_HI(*ax)-1);
		SET_FLAG(AF);
		SET_FLAG(CF);
	}else{
		SET_FLAG(AF);
		SET_FLAG(CF);
	}
	CLR_NIB(ax,1);
}

//RM means just involving registers and memory
//RMI means involving registers, memory, or immediates

void ADC_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg_a;
	s16 *reg_b;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm);
			res=*reg_a + MEM[abs_addr] + GET_FLAG(CF);
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg_a+MEM[abs_addr],GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
		case 1:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm8;
			res=*reg_a + MEM[abs_addr] + GET_FLAG(CF);
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg_a+MEM[abs_addr],GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
		case 2:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm16;
			res=*reg_a + MEM[abs_addr] + GET_FLAG(CF);
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg_a+MEM[abs_addr],GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
			break;
		case 3:
			reg_a=REG(RRR(rrr));
			reg_b=REG(RRR(mmm));
			if(d){ *reg_b = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,*reg_b)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg_a+*reg_b,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void ADC_Acc_Imm(u8 w,s8 imm8,s16 imm16){
	s16*reg=REG(AX);
	u32 res;
	if(w){
		res=*reg+imm16 + GET_FLAG(CF);
		(OVERFLOW_SUM(*reg,imm16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+imm16,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
	}
	else{
		res=*reg+imm8 + GET_FLAG(CF);
		(OVERFLOW_SUM(*reg,imm8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+imm8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void ADC_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	s16 * reg;
	u32 abs_addr;
	u32 res;
	if(s){
		if(w){
			res=*reg+(s16)imm8+GET_FLAG(CF);
			(OVERFLOW_SUM(*reg,(s16)imm8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+(s16)imm8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		}
		else{
			res=*reg+imm8+GET_FLAG(CF);
			(OVERFLOW_SUM(*reg,imm8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+imm8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		}
	}
	else{
			res=*reg+imm16+GET_FLAG(CF);
			(OVERFLOW_SUM(*reg,imm16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+imm16,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
	}
	FLAG_CHECK(res,1,1,1,1,1);
}




/*
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
/*

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
	MEM[SP_]=FLAGS;
	SP_-=2;
	MEM[SP_]=CS;
	SP_-=2;
	MEM[SP_]=IP+1;
	SP_-=2;
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

void IRET(){
	IP=MEM[SP_];
	SP_+=2;
	CS=MEM[SP_];
	SP_+=2;
	IP=MEM[SP_];
}

void JA(u32 eff_add){
	if(!GET_FLAG(CF) && !GET_FLAG(ZF)) IP=eff_add;
}

void JAE(u32 eff_add){
	if(!GET_FLAG(CF)) IP=eff_add;
}
void JB(u32 eff_add){
	if(GET_FLAG(CF)) IP=eff_add;
}

void JBE(u32 eff_add){
	if(GET_FLAG(CF) || GET_FLAG(ZF)) IP=eff_add;
}

void JC(u32 eff_add){
	if(GET_FLAG(CF)) IP=eff_add;
}

void JCXZ(u32 eff_add){
	if(!GET_FLAG(CX)) IP=eff_add;
}

void JE(u32 eff_add){
	if(GET_FLAG(ZF)) IP=eff_add;
}
void JG(u32 eff_add){
	if(!GET_FLAG(ZF) && !GET_FLAG(OF)) IP=eff_add;
}
void JGE(u32 eff_add){
	if((GET_FLAG(SF) && GET_FLAG(OF)) || (!GET_FLAG(SF) || !GET_FLAG(OF))) IP=eff_add;
}
void JL(u32 eff_add){
	if(GET_FLAG(SF) || GET_FLAG(OF)) IP=eff_add;
}
void JLE(u32 eff_add){
	if(GET_FLAG(SF) || GET_FLAG(OF) || GET_FLAG(ZF)) IP=eff_add;
}
void JMP(u32 eff_add){
	IP=eff_add;
}
void JNA(u32 eff_add){
	if(GET_FLAG(CF) || GET_FLAG(ZF)) IP=eff_add;
}
void JNAE(u32 eff_add){
	if(GET_FLAG(CF)) IP=eff_add;
}
void JNB(u32 eff_add){
	if(!GET_FLAG(CF)) IP=eff_add;
}
void JNBE(u32 eff_add){
	if(!GET_FLAG(CF) && !GET_FLAG(ZF)) IP=eff_add;
}
void JNC(u32 eff_add){
	if(!GET_FLAG(CF)) IP=eff_add;
}
void JNE(u32 eff_add){
	if(!GET_FLAG(ZF)) IP=eff_add;
}
void JNG(u32 eff_add){
	if(GET_FLAG(ZF) && (GET_FLAG(SF) || GET_FLAG(OF))) IP=eff_add;
}
void JNGE(u32 eff_add){
	if(GET_FLAG(SF) || GET_FLAG(OF)) IP=eff_add;
}
void JNL(u32 eff_add){
	if((GET_FLAG(SF) && GET_FLAG(OF)) || (!GET_FLAG(SF) || !GET_FLAG(OF))) IP=eff_add;
}
void JNLE(u32 eff_add){
	if( ((GET_FLAG(SF) && GET_FLAG(OF)) || (!GET_FLAG(SF) || !GET_FLAG(OF))) && !GET_FLAG(ZF) ) IP=eff_add;
}
void JNO(u32 eff_add){
	if(!GET_FLAG(OF)) IP=eff_add;
}
void JNP(u32 eff_add){
	if(!GET_FLAG(PF)) IP=eff_add;
}
void JNS(u32 eff_add){
	if(!GET_FLAG(SF)) IP=eff_add;
}
void JNZ(u32 eff_add){
	if(!GET_FLAG(ZF)) IP=eff_add;
}
void JO(u32 eff_add){
	if(GET_FLAG(OF)) IP=eff_add;
}
void JP(u32 eff_add){
	if(GET_FLAG(PF)) IP=eff_add;
}
void JPE(u32 eff_add){
	if(GET_FLAG(PF)) IP=eff_add;
}
void JPO(u32 eff_add){
	if(!GET_FLAG(PF)) IP=eff_add;
}
void JS(u32 eff_add){
	if(GET_FLAG(SF)) IP=eff_add;
}
void JZ(u32 eff_add){
	if(GET_FLAG(ZF)) IP=eff_add;
}

void LAHF(){
	SET_LOW(&AX,FLAGS & 0x00ff);
}

void LDS_Reg_Mem(s16* reg,u32 eff_add){
	*reg=MEM[eff_add];
	DS=MEM[eff_add+4];
}

void LEA_Mem_Reg(s16* reg,u32 eff_add){
	*reg=eff_add;
}

void LES(s16* reg,u32 eff_add){
	*reg=MEM[eff_add];
	ES=MEM[eff_add+4];
}
void LODSB(){
	SET_LOW(&AX,MEM[DS+SI]);
	(GET_FLAG(DF)) ? SI++ : SI--;
}

void LODSW(){
	AX=MEM[DS+SI];
	(GET_FLAG(DF)) ? SI++ : SI--;
}

void LOOP(u32 eff_add){
	CX--;
	if(!CX) IP=eff_add;
}

void LOOPE(u32 eff_add){
	CX--;
	if(!CX && GET_FLAG(ZF)) IP=eff_add;
}

void LOOPNE(u32 eff_add){
	CX--;
	if(!CX && !GET_FLAG(ZF)) IP=eff_add;
}

void LOOPZ(u32 eff_add){
	CX--;
	if(!CX && !GET_FLAG(ZF)) IP=eff_add;
}


void MOV_Reg_Mem(s16* reg,u32 eff_add){
	*reg=MEM[eff_add];
}
void MOV_Mem_Reg(u32 eff_add,s16* reg){
	MEM[eff_add]=*reg;
}
void MOV_Reg_Reg(s16* reg_a,s16* reg_b){
	*reg_a=*reg_b;
}
void MOV_Mem_Imm(u32 eff_add,s16 imm){
	MEM[eff_add]=imm;
}
void MOV_Reg_Imm(s16* reg,s16 imm){
	*reg=imm;
}
void MOV_SReg_Mem(s16* reg,u32 eff_add){
	*reg=MEM[eff_add];
}
void MOV_Mem_SReg(u32 eff_add,s16* reg){
	MEM[eff_add]=*reg;
}
void MOV_Reg_SReg(s16* reg_a,s16* reg_b){
	*reg_a=*reg_b;
}
void MOV_SReg_Reg(s16* reg_a,s16* reg_b){
	*reg_a=*reg_b;
}

void MOVSB(){
	MEM[ES+DI]=MEM[DS+SI];
	if(!GET_FLAG(DF)){SI++; DI++;}
	else{SI--; DI--;}
}

void MOVSW(){
	MEM[ES+DI]=MEM[DS+SI];
	if(!GET_FLAG(DF)){SI+=2; DI+=2;}
	else{SI-=2; DI-=2;}
}

void MUL_Reg_8b(s16* reg){
	AX=((u16)GET_LOW(AX))*((u16)GET_LOW(*reg));
	if(!DX){ CLR_FLAG(CF); CLR_FLAG(OF);}
	else {SET_FLAG(CF); SET_FLAG(OF);}
}
void MUL_Mem_8b(u32 eff_add){
	AX=((u16)GET_LOW(AX))*((u16)MEM[eff_add]);
	if(!DX){ CLR_FLAG(CF); CLR_FLAG(OF);}
	else {SET_FLAG(CF); SET_FLAG(OF);}
}

void MUL_Reg_16b(s16* reg){
	u32 res=((u16)GET_LOW(AX))*((u16)GET_LOW(*reg));
	DX=0x00ff & (res >> 8);
	AX=res;
	if(!DX){ CLR_FLAG(CF); CLR_FLAG(OF);}
	else {SET_FLAG(CF); SET_FLAG(OF);}
}
void MUL_Mem_16b(u32 eff_add){
	u32 res=((u16)GET_LOW(AX))*((u16)MEM[eff_add]);
	if(!DX){ CLR_FLAG(CF); CLR_FLAG(OF);}
	else {SET_FLAG(CF); SET_FLAG(OF);}
}

void NEG_Reg(s16* reg){
	s16 temp=*reg;
	*reg=0;
	ADD_Reg_Imm(reg,~(temp)+1);
}
void NEG_Mem(u32 eff_add){
	s16 temp=MEM[eff_add];
	MEM[eff_add]=0;
	ADD_Mem_Imm(eff_add,~(temp)+1);
}

void NOP(){return;}

void OR_Reg_Mem(s16* reg,u32 eff_add){
	*reg= *reg | MEM[eff_add];
	(*reg) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(*reg)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(*reg)) ? SET_FLAG(PF) : CLR_FLAG(PF);	
}
void OR_Mem_Reg(u32 eff_add,s16* reg){
	MEM[eff_add]= *reg | MEM[eff_add];
	(MEM[eff_add]) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(MEM[eff_add])) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(MEM[eff_add])) ? SET_FLAG(PF) : CLR_FLAG(PF);
}
void OR_Reg_Reg(s16* reg_a,s16* reg_b){
	*reg_a= *reg_a | *reg_b;
	(*reg_a) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(*reg_a)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(*reg_a)) ? SET_FLAG(PF) : CLR_FLAG(PF);
}
void OR_Mem_Imm(u32 eff_add,s16 imm){
	MEM[eff_add]= imm | MEM[eff_add];
	(MEM[eff_add]) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(MEM[eff_add])) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(MEM[eff_add])) ? SET_FLAG(PF) : CLR_FLAG(PF);
}
void OR_Reg_Imm(s16* reg,s16 imm){
	*reg= *reg | imm;
	(*reg) ? SET_FLAG(ZF) : CLR_FLAG(ZF);
	(SIGN(*reg)) ? SET_FLAG(SF) : CLR_FLAG(SF);
	(PARITY(*reg)) ? SET_FLAG(PF) : CLR_FLAG(PF);	
}

*/

