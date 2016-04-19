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

u8 OVERFLOW_OR(u16 a,u16 b){
	if((SIGN(a) && SIGN(b)) || (!SIGN(a) && !SIGN(b))){
		if(SIGN(a) && !(SIGN(a | b))){ return 1;}
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

u32 EA(u8 mmm){
	switch(mmm){
		case 0:
			return BX+SI;
			break;
		case 1:
			return BX+DI;
			break;
		case 2:
			return BP+SI;
			break;
		case 3:
			return BP+DI;
			break;
		case 4:
			return SI;
			break;
		case 5:
			return DI;
			break;
		case 6:
			return BP;
			break;
		case 7:
			return BX;
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

u8 SSS(u8 sss){
	switch(sss){
		case 0:
			return ES;
			break;
		case 1:
			return CS;
			break;
		case 2:
			return SS;
			break;
		case 3:
			return DS;
			break;
		case 5:
			return 0;
			break;
		case 6:
			return 0;
			break;
		case 7:
			return 0;
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
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			reg_a=REG(RRR(rrr));
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
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
			if(w){
				if(d){ *reg_b=res;}
				else{ *reg_a=res;}
			}
			else{
				if(d){ SET_LOW(reg_b,res);}
				else{ SET_LOW(reg_a,res);}
			}
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
		*reg=res;
	}
	else{
		res=*reg+imm8 + GET_FLAG(CF);
		(OVERFLOW_SUM(*reg,imm8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+imm8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		SET_LOW(reg,res);
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void ADC_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 * reg;
	s16 * ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 1:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 2:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 3:
			abs_addr=MMM(mmm)+imm16;
			break;

	}
	if(0<=oo && oo<=3){
		if(s){
			if(w){
				res=MEM[abs_addr] + (s16)imm_dat8 + GET_FLAG(CF);
				(OVERFLOW_SUM(MEM[abs_addr],(s16)imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				if(!GET_FLAG(OF)){
					(OVERFLOW_SUM(MEM[abs_addr]+(s16)imm_dat8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
				}
			}
			else{
				res=MEM[abs_addr] + imm_dat8 + GET_FLAG(CF);
				(OVERFLOW_SUM(MEM[abs_addr],imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				if(!GET_FLAG(OF)){
					(OVERFLOW_SUM(MEM[abs_addr]+imm_dat8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
				}
			}
		}
		else{
			res=MEM[abs_addr] + imm_dat16 + GET_FLAG(CF);
			(OVERFLOW_SUM(MEM[abs_addr],imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(MEM[abs_addr]+imm_dat16,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		}
	}
	else{
		if(s){
			if(w){
				res=*reg + (s16)imm_dat8 + GET_FLAG(CF);
				(OVERFLOW_SUM(*reg,(s16)imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				if(!GET_FLAG(OF)){
					(OVERFLOW_SUM(*reg+(s16)imm_dat8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
				}
				*reg=res;
			}
			else{
				res=*reg + imm_dat8 + GET_FLAG(CF);
				(OVERFLOW_SUM(*reg,imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				if(!GET_FLAG(OF)){
					(OVERFLOW_SUM(*reg+imm_dat8,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
				}
				SET_LOW(reg,res);
			}
		}
		else{
			res=*reg + imm_dat16 + GET_FLAG(CF);
			(OVERFLOW_SUM(*reg,imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			if(!GET_FLAG(OF)){
				(OVERFLOW_SUM(*reg+imm_dat16,GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			*reg=res;
		}
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void ADD_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg_a;
	s16 *reg_b;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			reg_a=REG(RRR(rrr));
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			res=*reg_a + MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
		case 1:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm8;
			res=*reg_a + MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
		case 2:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm16;
			res=*reg_a + MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_SUM(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
			break;
		case 3:
			reg_a=REG(RRR(rrr));
			reg_b=REG(RRR(mmm));
			res=*reg_a + *reg_b;
			if(w){
				if(d){ *reg_b=res;}
				else{ *reg_a=res;}
			}
			else{
				if(d){ SET_LOW(reg_b,res);}
				else{ SET_LOW(reg_a,res);}
			}
			(OVERFLOW_SUM(*reg_a,*reg_b)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void ADD_Acc_Imm(u8 w,s8 imm8,s16 imm16){
	s16*reg=REG(AX);
	u32 res;
	if(w){
		res=*reg+imm16;
		(OVERFLOW_SUM(*reg,imm16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		*reg=res;
	}
	else{
		res=*reg+imm8;
		(OVERFLOW_SUM(*reg,imm8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_LOW(reg,res);
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void ADD_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 * reg;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 1:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 2:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 3:
			abs_addr=MMM(mmm)+imm16;
			break;

	}
	if(0<=oo && oo<=3){
		if(s){
			if(w){
				res=MEM[abs_addr] + (s16)imm_dat8;
				(OVERFLOW_SUM(MEM[abs_addr],(s16)imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{
				res=MEM[abs_addr] + imm_dat8;
				(OVERFLOW_SUM(MEM[abs_addr],imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		}
		else{
			res=MEM[abs_addr] + imm_dat16;
			(OVERFLOW_SUM(MEM[abs_addr],imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		}
	}
	else{
		if(s){
			if(w){
				res=*reg + (s16)imm_dat8;
				(OVERFLOW_SUM(*reg,(s16)imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				*reg=res;
			}
			else{
				res=*reg + imm_dat8;
				(OVERFLOW_SUM(*reg,imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				SET_LOW(reg,res);
			}
		}
		else{
			res=*reg + imm_dat16;
			(OVERFLOW_SUM(*reg,imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			*reg=res;
		}
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void AND_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg_a;
	s16 *reg_b;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			res=*reg_a & MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			break;
		case 1:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm8;
			res=*reg_a & MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			break;
		case 2:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm16;
			res=*reg_a & MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			break;
			break;
		case 3:
			reg_a=REG(RRR(rrr));
			reg_b=REG(RRR(mmm));
			res=*reg_a & *reg_b;
			if(w){
				if(d){ *reg_b=res;}
				else{ *reg_a=res;}
			}
			else{
				if(d){ SET_LOW(reg_b,res);}
				else{ SET_LOW(reg_a,res);}
			}
			break;
	}
	CLR_FLAG(CF);
	CLR_FLAG(OF);
	FLAG_CHECK(res,0,1,1,1,0);
}

void AND_Acc_Imm(u8 w,s8 imm8,s16 imm16){
	s16*reg=REG(AX);
	u32 res;
	if(w){
		res=*reg & imm16;
		*reg=res;
	}
	else{
		res=*reg & imm8;
		SET_LOW(reg,res);
	}
	CLR_FLAG(CF);
	CLR_FLAG(OF);
	FLAG_CHECK(res,0,1,1,1,0);
}

void AND_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 * reg;
	s16 * ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 1:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 2:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 3:
			abs_addr=MMM(mmm)+imm16;
			break;

	}
	if(0<=oo && oo<=3){
		if(s){
			if(w){
				res=MEM[abs_addr] & (s16)imm_dat8;
			}
			else{
				res=MEM[abs_addr] & imm_dat8;
			}
		}
		else{
			res=MEM[abs_addr] & imm_dat16;
		}
	}
	else{
		if(s){
			if(w){
				res=*reg & (s16)imm_dat8;
				*reg=res;
			}
			else{
				res=*reg & imm_dat8;
				SET_LOW(reg,res);
			}
		}
		else{
			res=*reg & imm_dat16;
			*reg=res;
		}
	}
}


void CBW(){
	s16 *ax=REG(AX);
	(GET_HI(*ax)) ? SET_HI(ax,255) : SET_HI(ax,0);
}

void CLC(){
	CLR_FLAG(CF);
}

void CLD(){
	CLR_FLAG(DF);
}
void CLI(){
	CLR_FLAG(IF);
}
void CMC(){
	TOGGLE_FLAG(CF);
}

void CMP_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg_a;
	s16 *reg_b;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			reg_a=REG(RRR(rrr));
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			if(d){ 
				res=MEM[abs_addr] + TC(*reg_a);
				MEM[abs_addr] = res;
				(OVERFLOW_SUM(TC(*reg_a),MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{ 
				res=*reg_a + TC(MEM[abs_addr]);
				*reg_a=res;
				(OVERFLOW_SUM(*reg_a,TC(MEM[abs_addr]))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
		case 1:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm8;
			if(d){ 
				res=MEM[abs_addr] + TC(*reg_a);
				MEM[abs_addr] = res;
				(OVERFLOW_SUM(TC(*reg_a),MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{ 
				res=*reg_a + TC(MEM[abs_addr]);
				*reg_a=res;
				(OVERFLOW_SUM(*reg_a,TC(MEM[abs_addr]))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
		case 2:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm16;
			if(d){ 
				res=MEM[abs_addr] + TC(*reg_a);
				MEM[abs_addr] = res;
				(OVERFLOW_SUM(TC(*reg_a),MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{ 
				res=*reg_a + TC(MEM[abs_addr]);
				*reg_a=res;
				(OVERFLOW_SUM(*reg_a,TC(MEM[abs_addr]))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
		case 3:
			reg_a=REG(RRR(rrr));
			reg_b=REG(RRR(mmm));
			if(d){ 
				res=*reg_b + TC(*reg_a);
				*reg_b = res;
				(OVERFLOW_SUM(TC(*reg_a),*reg_b)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{ 
				res=*reg_a + TC(*reg_b);
				*reg_a=res;
				(OVERFLOW_SUM(*reg_a,TC(*reg_b))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			break;
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void CMP_Acc_Imm(u8 w,s8 imm8,s16 imm16){
	s16*reg=REG(AX);
	u32 res;
	if(w){
		res=*reg+TC(imm16);
		(OVERFLOW_SUM(*reg,TC(imm16))) ? SET_FLAG(OF) : CLR_FLAG(OF);
		*reg=res;
	}
	else{
		res=*reg+TC(imm8);
		(OVERFLOW_SUM(*reg,TC(imm8))) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_LOW(reg,res);
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void CMP_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 * reg;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 1:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 2:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 3:
			abs_addr=MMM(mmm)+imm16;
			break;

	}
	if(0<=oo && oo<=3){
		if(s){
			if(w){
				res=MEM[abs_addr] + TC((s16)imm_dat8);
				(OVERFLOW_SUM(MEM[abs_addr],TC((s16)imm_dat8))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{
				res=MEM[abs_addr] + TC(imm_dat8); 
				(OVERFLOW_SUM(MEM[abs_addr],TC(imm_dat8))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		}
		else{
			res=MEM[abs_addr] + TC(imm_dat16);
			(OVERFLOW_SUM(MEM[abs_addr],imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		}
	}
	else{
		if(s){
			if(w){
				res=*reg + TC((s16)imm_dat8);
				(OVERFLOW_SUM(*reg,TC((s16)imm_dat8))) ? SET_FLAG(OF) : CLR_FLAG(OF);
				*reg=res;
			}
			else{
				res=*reg + TC(imm_dat8);
				(OVERFLOW_SUM(*reg,TC(imm_dat8))) ? SET_FLAG(OF) : CLR_FLAG(OF);
				SET_LOW(reg,res);
			}
		}
		else{
			res=*reg + TC(imm_dat16);
			(OVERFLOW_SUM(*reg,TC(imm_dat16))) ? SET_FLAG(OF) : CLR_FLAG(OF);
			*reg=res;
		}
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void CMPSB(){
	u32 res=MEM[ABS(DS,SI)]+TC(MEM[ABS(ES,DI)]);
	s16 *si=REG(SI);
	s16 *di=REG(DI);
	OVERFLOW_SUM(MEM[ABS(DS,SI)],TC(MEM[ABS(ES,DI)]));
	FLAG_CHECK(res,1,1,1,1,1);
	if(GET_FLAG(DF)){
		(*si)++;
		(*di)++;
	}else{
		(*si)--;
		(*di)--;
	}
}
void CMPSW(){
	u32 res=MEM[ABS(DS,SI)]+TC(MEM[ABS(ES,DI)]);
	s16 *si=REG(SI);
	s16 *di=REG(DI);
	OVERFLOW_SUM(MEM[ABS(DS,SI)],TC(MEM[ABS(ES,DI)]));
	FLAG_CHECK(res,1,1,1,1,1);
	if(GET_FLAG(DF)){
		(*si)+=2;
		(*di)+=2;
	}else{
		(*si)-=2;
		(*di)-=2;
	}
}
void CWD(){
	s16* ax=REG(AX);
	s16 *dx=REG(DX);
	if(GET_HI(*ax)){*dx=0xffff;}
	else *dx=0;
}
void DAA(){
	s16 * ax=REG(AX);
	s16 temp=*ax;
	if((NIBBLE(*ax)>9) || GET_FLAG(AF)){
		SET_LOW(ax,GET_LOW(*ax)+6);
		SET_FLAG(AF);
	}
	if((NIBBLE(*ax)>(0x9f)) || GET_FLAG(CF)){
		SET_LOW(ax,GET_LOW(*ax)+0x60);
		SET_FLAG(CF);
	}
	(!((SIGN(temp) && SIGN(*ax)) || (!SIGN(temp) && !SIGN(*ax))))  ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(*ax,1,1,1,1,1);
}
void DAS(){
	s16 * ax=REG(AX);
	s16 temp=*ax;
	if((NIBBLE(*ax)>9) || GET_FLAG(AF)){
		SET_LOW(ax,GET_LOW(*ax)+TC(6));
		SET_FLAG(AF);
	}
	if((NIBBLE(*ax)>(0x9f)) || GET_FLAG(CF)){
		SET_LOW(ax,GET_LOW(*ax)+TC(0x60));
		SET_FLAG(CF);
	}
	(!((SIGN(temp) && SIGN(*ax)) || (!SIGN(temp) && !SIGN(*ax))))  ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(*ax,1,1,1,1,1);
}

//W bit doesn't matter for DEC or INC

void DEC(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	s16* reg;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]-1;
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]-1;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]-1;
			break;
		case 3:
			reg=REG(mmm);
			res=*reg;
			*reg=*reg-1;
			break;
	}
	(OVERFLOW_SUM(res,TC(1))) ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(res+TC(1),0,1,1,1,1);
}

void DIV(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	s16 *reg;
	s16 *ax=REG(AX);
	s16 *ds;
	u32 abs_addr;
	s16 *dx=REG(DX);
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
		case 3:
			reg=REG(mmm);
			break;
	}
	if(0<=oo && oo>=3){
		if(w){
			temp = ((*dx << 8) & 0xff00) | *ax;
			*ax=(u16)(temp/MEM[abs_addr]);
			*dx=(u16)(temp%MEM[abs_addr]);
		}
		else{
			SET_LOW(ax,(u8)((*ax)/MEM[abs_addr]));
			SET_HI(ax,(u8)((*ax)%MEM[abs_addr]));
		}
	}else{
		if(w){
			temp = ((*dx << 8) & 0xff00) | *ax;
			*ax=(u16)(temp/(*reg));
			*dx=(u16)(temp%(*reg));
		}
		else{
			SET_LOW(ax,(u8)((*ax)/(*reg)));
			SET_HI(ax,(u8)((*ax)%(*reg)));
		}

	}
}

void IDIV(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	s16 *reg;
	s16 *ax=REG(AX);
	s16 *ds;
	u32 abs_addr;
	s16 *dx=REG(DX);
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
		case 3:
			reg=REG(mmm);
			break;
	}
	if(0<=oo && oo>=3){
		if(w){
			temp = ((*dx << 8) & 0xff00) | *ax;
			*ax=temp/MEM[abs_addr];
			*dx=temp%MEM[abs_addr];
		}
		else{
			SET_LOW(ax,(*ax)/MEM[abs_addr]);
			SET_HI(ax,(*ax)%MEM[abs_addr]);
		}
	}else{
		if(w){
			temp = ((*dx << 8) & 0xff00) | *ax;
			*ax=temp/(*reg);
			*dx=temp%(*reg);
		}
		else{
			SET_LOW(ax,(*ax)/(*reg));
			SET_HI(ax,(*ax)%(*reg));
		}

	}
}

void IMUL(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	u32 res;
	s16 *reg;
	s16 *ax=REG(AX);
	s16 *ds;
	u32 abs_addr;
	s16 *dx=REG(DX);
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
		case 3:
			reg=REG(mmm);
			break;
	}
	if(0<=oo && oo>=3){
		if(w){
			temp=*ax;
			*ax=temp*MEM[abs_addr];
			*dx=(temp*MEM[abs_addr] >> 16) & 0x0000ffff;
		}
		else{
			SET_LOW(ax,GET_LOW(*ax)*MEM[abs_addr]);
		}
	}else{
		if(w){
			temp=*ax;
			*ax=temp*(*reg);
			*dx=(temp*(*reg) >> 16) & 0x0000ffff;
		}
		else{
			SET_LOW(ax,GET_LOW(*ax)*(*reg));
		}

	}
	(!((SIGN(temp) && SIGN(*ax)) || (!SIGN(temp) && !SIGN(*ax))))  ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(*ax,1,1,1,1,1);
}

void INC(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	s16* reg;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]+1;
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]+1;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]+1;
			break;
		case 3:
			reg=REG(mmm);
			res=*reg;
			*reg=*reg+1;
			break;
	}
	(OVERFLOW_SUM(res,1)) ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(res+1,0,1,1,1,1);
}

void INT10(){
	u8 ah=GET_HI(*REG(AX));
	switch(ah){
		case 0:
			break;
		case 12:
			set_pixel(*REG(CX),*REG(DX),GET_LOW(*REG(AX)));
			break;
	}
}


void IRET(){
	s16 *sp=REG(SP);
	s16 *ip=REG(IP);
	s16 *cs=REG(CS);
	s16 *flags=REG(FLAGS);
	*ip=MEM[*sp]+1;
	(*sp)+=2;
	*cs=MEM[*sp];
	(*sp)+=2;
	*flags=MEM[*sp];
	(*sp)+=2;
}

void LAHF(){
	s16 *ax=REG(AX);
	SET_HI(ax,*REG(FLAGS));
}

void LDS(u8 oo, u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg=REG(RRR(rrr));
	s16 *ds=REG(DS);
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
	}
	*reg=MEM[abs_addr];
	*ds=MEM[abs_addr+2];	
}

void LES(u8 oo, u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg=REG(RRR(rrr));
	s16 *ds=REG(DS);
	s16 *es=REG(ES);
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
	}
	*reg=MEM[abs_addr];
	*es=MEM[abs_addr+2];	
}

void LEA(u8 oo, u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg=REG(RRR(rrr));
	s16 *ds=REG(DS);
	s16 *es=REG(ES);
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ abs_addr=imm16;}
			else {abs_addr=EA(mmm);}
			break;
		case 1:
			abs_addr=EA(mmm) + imm8;
			break;
		case 2:
			abs_addr=EA(mmm) + imm16;
			break;
	}
	*reg=MEM[abs_addr];
}

void LODSB(){
	s16 *si=REG(SI);
	s16 *ds=REG(DS);
	s16 *ax=REG(AX);
	SET_LOW(ax,MEM[ABS(*ds,*si)]);
	(GET_FLAG(DF)) ? (*si)++: (*si)--;
}

void LODSW(){
	s16 *si=REG(SI);
	s16 *ds=REG(DS);
	s16 *ax=REG(AX);
	SET_LOW(ax,MEM[ABS(*ds,*si)]);
	if(GET_FLAG(DF)){(*si)+=2;} else{(*si)-=2;}
}

void MOV_MOFS_Acc(u8 d,u8 w,s8 imm8,s16 imm16){
	s16 *ax=REG(AX);
	if(d){
		if(w){ MEM[ABS(CS,imm8)]=*ax;}
		else{ MEM[ABS(CS,imm16)]=*ax;}
	}
	else{
		if(w){ *ax=MEM[ABS(CS,imm8)];}
		else{ *ax=MEM[ABS(CS,imm16)]=*ax;}
	}
}

void MOV_Reg_Imm(u8 w,u8 rrr,s8 imm8,s16 imm16){
	s16 *reg=REG(RRR(rrr));
	if(w){
		*reg=imm16;
	}
	else{
		*reg=imm8;
	}
}

void MOV_Mem_Imm(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 *reg;
	s16 *ds;
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
	}
	if(w){
		MEM[abs_addr]=imm_dat16;
	}
	else{
		MEM[abs_addr]=imm_dat8;
	}
	
}

void MOV_Reg_Reg(u8 w,u8 rrr,u8 mmm){
	s16 *reg_a=REG(RRR(rrr));
	s16 *reg_b=REG(RRR(mmm));
	*reg_a=*reg_b; //Check w bit later
}

void MOV_Reg_Mem(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	u32 abs_addr;
	s16 *reg=REG(RRR(rrr));
	s16 *ds=REG(DS);
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
	}
	if(d){
		MEM[abs_addr]=*reg;
	}
	else{
		*reg=MEM[abs_addr];
	}
}

void MOV_Reg_Seg(u8 d,u8 w,u8 sss,u8 mmm){
	s16 *reg_a=REG(SSS(sss));
	s16 *reg_b=REG(RRR(mmm));
	if(d){
		*reg_b=*reg_a;
	}
	else{
		*reg_a=*reg_b;
	}
}

void MOV_Mem_Seg(u8 d,u8 oo,u8 sss,u8 mmm,s8 imm8,s16 imm16){
	u32 abs_addr;
	s16 *reg=REG(SSS(sss));
	s16 *ds=REG(DS);
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
	}
	if(d){
		MEM[abs_addr]=*reg;
	}
	else{
		*reg=MEM[abs_addr];
	}
}

void MOVSB(){
	s16 *si=REG(SI);
	s16 *ds=REG(DS);
	s16 *di=REG(DI);
	s16 *es=REG(ES);
	MEM[ABS(*es,*di)]=MEM[ABS(*ds,*si)];
	if(!GET_FLAG(DF)){(*si)++;} else{(*si)--;}
}

void MOVSW(){
	s16 *si=REG(SI);
	s16 *ds=REG(DS);
	s16 *di=REG(DI);
	s16 *es=REG(ES);
	MEM[ABS(*es,*di)]=MEM[ABS(*ds,*si)];
	if(!GET_FLAG(DF)){(*si)+=2;} else{(*si)-=2;}
}

void MUL(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	u32 res;
	s16 *reg;
	s16 *ax=REG(AX);
	s16 *ds;
	u32 abs_addr;
	s16 *dx=REG(DX);
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
		case 3:
			reg=REG(mmm);
			break;
	}
	if(0<=oo && oo>=3){
		if(w){
			temp=*ax;
			*ax=(s16)temp*MEM[abs_addr];
			*dx=(s16)((temp*MEM[abs_addr] >> 16) & 0x0000ffff);
		}
		else{
			SET_LOW(ax,GET_LOW(*ax)*MEM[abs_addr]);
		}
	}else{
		if(w){
			temp=*ax;
			*ax=temp*(*reg);
			*dx=(temp*(*reg) >> 16) & 0x0000ffff;
		}
		else{
			SET_LOW(ax,GET_LOW(*ax)*(*reg));
		}

	}
	(!((SIGN(temp) && SIGN(*ax)) || (!SIGN(temp) && !SIGN(*ax))))  ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(*ax,1,1,1,1,1);
}

void NEG(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	u32 res;
	s16 *reg=REG(MMM(mmm));
	s16 *ds;
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
		case 3:
			reg=REG(mmm);
			break;
	}
	temp=*reg;
	if(0<=oo && oo>=3){
		res=TC(MEM[abs_addr]);
		MEM[abs_addr]=TC(MEM[abs_addr]);
		
	}else{
		res=TC(*reg);
		if(w){
			*reg=res;
		}
		else{
			SET_LOW(reg,TC(GET_LOW(*reg)));
		}

	}
	(!((SIGN(temp) && SIGN(res)) || (!SIGN(temp) && !SIGN(res))))  ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(TC(*reg),1,1,1,1,1);
}

void NOT(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	u32 res;
	s16 *reg=REG(MMM(mmm));
	s16 *ds;
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm) + imm8;
			break;
		case 2:
			abs_addr=MMM(mmm) + imm16;
			break;
		case 3:
			reg=REG(mmm);
			break;
	}
	temp=*reg;
	if(0<=oo && oo>=3){
		res=~(MEM[abs_addr]);
		MEM[abs_addr]=~(MEM[abs_addr]);
		
	}else{
		res=~(*reg);
		if(w){
			*reg=res;
		}
		else{
			SET_LOW(reg,~(GET_LOW(*reg)));
		}

	}
	(!((SIGN(temp) && SIGN(res)) || (!SIGN(temp) && !SIGN(res))))  ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(~(*reg),1,1,1,1,1);
}

void OR_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg_a;
	s16 *reg_b;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			reg_a=REG(RRR(rrr));
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			res=*reg_a | MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_OR(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
		case 1:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm8;
			res=*reg_a | MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_OR(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
		case 2:
			reg_a=REG(RRR(rrr));
			abs_addr=MMM(mmm)+imm16;
			res=*reg_a | MEM[abs_addr];
			if(d){ MEM[abs_addr] = res;}
			else{ *reg_a=res;}
			(OVERFLOW_OR(*reg_a,MEM[abs_addr])) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
			break;
		case 3:
			reg_a=REG(RRR(rrr));
			reg_b=REG(RRR(mmm));
			res=*reg_a | *reg_b;
			if(w){
				if(d){ *reg_b=res;}
				else{ *reg_a=res;}
			}
			else{
				if(d){ SET_LOW(reg_b,res);}
				else{ SET_LOW(reg_a,res);}
			}
			(OVERFLOW_OR(*reg_a,*reg_b)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			break;
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void OR_Acc_Imm(u8 w,s8 imm8,s16 imm16){
	s16*reg=REG(AX);
	u32 res;
	if(w){
		res=*reg | imm16;
		(OVERFLOW_OR(*reg,imm16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		*reg=res;
	}
	else{
		res=*reg | imm8;
		(OVERFLOW_OR(*reg,imm8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		SET_LOW(reg,res);
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void OR_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 * reg;
	s16 *ds;
	u32 abs_addr;
	u32 res;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 2:
			abs_addr=MMM(mmm)+imm16;
			break;

	}
	if(0<=oo && oo<=3){
		if(s){
			if(w){
				res=MEM[abs_addr] | (s16)imm_dat8;
				(OVERFLOW_OR(MEM[abs_addr],(s16)imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
			else{
				res=MEM[abs_addr] | imm_dat8;
				(OVERFLOW_OR(MEM[abs_addr],imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			}
		}
		else{
			res=MEM[abs_addr] | imm_dat16;
			(OVERFLOW_OR(MEM[abs_addr],imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
		}
	}
	else{
		if(s){
			if(w){
				res=*reg | (s16)imm_dat8;
				(OVERFLOW_OR(*reg,(s16)imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				*reg=res;
			}
			else{
				res=*reg | imm_dat8;
				(OVERFLOW_OR(*reg,imm_dat8)) ? SET_FLAG(OF) : CLR_FLAG(OF);
				SET_LOW(reg,res);
			}
		}
		else{
			res=*reg | imm_dat16;
			(OVERFLOW_OR(*reg,imm_dat16)) ? SET_FLAG(OF) : CLR_FLAG(OF);
			*reg=res;
		}
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void POP_Reg_W(u8 rrr){
	s16* reg=REG(RRR(rrr));
	*reg=MEM[ABS(*REG(SS),*REG(SP))];
	(*REG(SP))+=2;
}

void POP_Mem_W(u8 oo,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg;
	s16 *ds=REG(DS);
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 2:
			abs_addr=MMM(mmm)+imm16;
			break;
	}
	MEM[abs_addr]=MEM[ABS(*REG(SS),*REG(SP))];
	(*REG(SP))+=2;
}

void POP_Seg(u8 sss){
	s16* reg=REG(SSS(sss));
	*reg=MEM[ABS(*REG(SS),*REG(SP))];
	(*REG(SP))+=2;
}

void POPF(){
	s16 *flags=REG(FLAGS);
	*flags=MEM[ABS(*REG(SS),*REG(SP))];
	(*REG(SP))+=2;
}

void PUSH_Reg_W(u8 rrr){
	s16* reg=REG(RRR(rrr));
	(*REG(SP))-=2;
	MEM[ABS(*REG(SS),*REG(SP))]=*reg;
}

void PUSH_Mem_W(u8 oo,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg;
	s16 *ds=REG(DS);
	u32 abs_addr;
	switch(oo){
		case 0:
			if(mmm == 6){ ds=REG(DS); abs_addr=ABS(*ds,imm16);}
			else {abs_addr=MMM(mmm);}
			break;
		case 1:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 2:
			abs_addr=MMM(mmm)+imm16;
			break;
	}
	(*REG(SP))-=2;
	MEM[ABS(*REG(SS),*REG(SP))]=MEM[abs_addr];
}

void PUSH_Seg(u8 sss){
	s16* reg=REG(SSS(sss));
	(*REG(SP))-=2;
	MEM[ABS(*REG(SS),*REG(SP))]=*reg;
}

void PUSHF(){
	s16* reg=REG(FLAGS);
	(*REG(SP))-=2;
	MEM[ABS(*REG(SS),*REG(SP))]=*reg;
}








