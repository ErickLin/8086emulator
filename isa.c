#include "isa.h"
#include <stdio.h>

typedef s16 (*func_2op)(s16, s16);

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

u8 OVERFLOW_XOR(u16 a,u16 b){
	if((SIGN(a) && SIGN(b)) || (!SIGN(a) && !SIGN(b))){
		if(SIGN(a) && !(SIGN(a ^ b))){ return 1;}
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

u8 RRR(u8 rrr) {
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
        /*
		case 4:
			return SP;
			break;
        */
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

u32 calc_abs_addr(u8 w, u8 oo, u8 mmm, s8 imm8, s16 imm16) {
    u32 abs_addr = MMM(mmm);
	switch(oo){
		case 0:
			if(mmm == 6){
                s16* ds=REG(DS);
                if (w) {
                    abs_addr=ABS(*ds,imm16);
                } else {
                    abs_addr=ABS(*ds,imm8);
                }
            } else {
                abs_addr=MMM(mmm);
            }
			break;
		case 1:
			abs_addr=MMM(mmm)+imm8;
			break;
		case 2:
			abs_addr=MMM(mmm)+imm16;
			break;
	}
    return abs_addr;
}

s16 adc(s16 left_op, s16 right_op) {
    (OVERFLOW_SUM(left_op, right_op)) ? SET_FLAG(OF) : CLR_FLAG(OF);
    if (!GET_FLAG(OF)) {
        (OVERFLOW_SUM(left_op + right_op, GET_FLAG(CF))) ? SET_FLAG(OF) : CLR_FLAG(OF);
    }
    return left_op + right_op + GET_FLAG(CF);
}

s16 add(s16 left_op, s16 right_op) {
    (OVERFLOW_SUM(left_op, right_op)) ? SET_FLAG(OF) : CLR_FLAG(OF);
    return left_op + right_op;
}

s16 and(s16 left_op, s16 right_op) {
    return left_op & right_op;
}

s16 cmp(s16 left_op, s16 right_op) {
    return left_op - right_op;
}

s16 or(s16 left_op, s16 right_op) {
    return left_op | right_op;
}

s16 sbb(s16 left_op, s16 right_op) {
    return left_op - right_op - GET_FLAG(CF);
}

s16 sub(s16 left_op, s16 right_op) {
    return left_op - right_op;
}

s16 xor(s16 left_op, s16 right_op) {
    return left_op ^ right_op;
}

s16 test(s16 left_op, s16 right_op) {
    return left_op & right_op;
}

void func_RM(func_2op func, u8 d, u8 w, u8 oo, u8 rrr, u8 mmm, s8 imm8, s16 imm16) {
    if (w) {
        s16 *left_op_ptr, *right_op_ptr;
        s16 res;
        u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
        if (oo >= 0 && oo <= 2) {
            if (d) {
                left_op_ptr = REG(RRR(rrr));
                right_op_ptr = &MEM[abs_addr];
            } else {
                left_op_ptr = &MEM[abs_addr];
                right_op_ptr = REG(RRR(rrr));
            }
            res = func(*left_op_ptr, *right_op_ptr);
            if (func != &cmp && func != &test) {
                *left_op_ptr = res;
            }
        } else {
            if (d) {
                left_op_ptr = REG(RRR(rrr));
                right_op_ptr = REG(RRR(mmm));
            } else { // should never happen
                left_op_ptr = REG(RRR(mmm));
                right_op_ptr = REG(RRR(rrr));
            }
            res = func(*left_op_ptr, *right_op_ptr);
            if (func != &cmp && func != &test) {
                *left_op_ptr = res;
            }
        }
        FLAG_CHECK(res,1,1,1,1,1);
    } else {
        s8 *left_op_ptr, *right_op_ptr;
        s8 res;
        u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
        if (oo >= 0 && oo <= 2) {
            if (d) {
                left_op_ptr = (s8*) REG(RRR(rrr % 4));
                if (rrr >= 4) {
                    left_op_ptr++;
                }
                right_op_ptr = (s8*) &MEM[abs_addr];
            } else {
                left_op_ptr = (s8*) &MEM[abs_addr];
                right_op_ptr = (s8*) REG(RRR(rrr % 4));
                if (rrr >= 4) {
                    right_op_ptr++;
                }
            }
            res = func(*left_op_ptr, *right_op_ptr);
            if (func != &cmp && func != &test) {
                *left_op_ptr = res;
            }
        } else {
            if (d) {
                left_op_ptr = (s8*) REG(RRR(rrr % 4));
                if (rrr >= 4) {
                    left_op_ptr++;
                }
                right_op_ptr = (s8*) REG(RRR(mmm));
            } else { // should never happen
                left_op_ptr = (s8*) REG(RRR(mmm));
                right_op_ptr = (s8*) REG(RRR(rrr % 4));
                if (rrr >= 4) {
                    right_op_ptr++;
                }
            }
            res = func(*left_op_ptr, *right_op_ptr);
            if (func != &cmp && func != &test) {
                *left_op_ptr = res;
            }
        }
        FLAG_CHECK(res,1,1,1,1,1);
    }
}

void func_Acc_Imm(func_2op func, u8 w, s8 imm8, s16 imm16) {
	s16 *reg = REG(AX);
    s16 res;
	if (w) {
        res = func(*reg, imm16);
        if (func != &cmp && func != &test) {
            *reg = res;
        }
	} else {
		res = func(*reg, imm8);
        if (func != &cmp && func != &test) {
            SET_LOW(reg, res);
        }
	}
	FLAG_CHECK(res,1,1,1,1,1);
}

void func_RMI(func_2op func, u8 s, u8 w, u8 oo, u8 mmm, s8 imm8, s16 imm16, s8 imm_dat8, s16 imm_dat16) {
    s16 *left_op_ptr;
    s16 res;
    if (oo >= 0 && oo <= 2) {
    	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
        left_op_ptr = &MEM[abs_addr];
    } else {
	    s16 *reg = REG(RRR(mmm));
        left_op_ptr = reg;
    }

    if (s) {
        if(w){
            res = func(*left_op_ptr, (s16)imm_dat8);
            if (func != &cmp && func != &test) {
                *left_op_ptr = res;
            }
        } else {
            res = func(*left_op_ptr, imm_dat8);
            if (func != &cmp && func != &test) {
                SET_LOW(left_op_ptr, res);
            }
        }
    } else {
        res = func(*left_op_ptr, imm_dat16);
        if (func != &cmp && func != &test) {
            *left_op_ptr = res;
        }
    }
	FLAG_CHECK(res,1,1,1,1,1);
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
    func_RM(&adc, d, w, oo, rrr, mmm, imm8, imm16);
}

void ADC_Acc_Imm(u8 w, s8 imm8, s16 imm16) {
    func_Acc_Imm(&adc, w, imm8, imm16);
}

void ADC_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&adc, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}

void ADD_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
    func_RM(&add, d, w, oo, rrr, mmm, imm8, imm16);
}

void ADD_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&add, w, imm8, imm16);
}

void ADD_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&add, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}

void AND_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
    func_RM(&and, d, w, oo, rrr, mmm, imm8, imm16);
}

void AND_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&add, w, imm8, imm16);
}

void AND_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&and, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
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
    func_RM(&cmp, d, w, oo, rrr, mmm, imm8, imm16);
}

void CMP_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&cmp, w, imm8, imm16);
}

void CMP_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&cmp, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
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
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
	u32 res;
	switch(oo){
		case 0:
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]-1;
			break;
		case 1:
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]-1;
			break;
		case 2:
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]-1;
			break;
		case 3:
			reg=REG(RRR(mmm));
			res=*reg;
			*reg=*reg-1;
			break;
	}
	(OVERFLOW_SUM(res,TC(1))) ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(res+TC(1),0,1,1,1,1);
}

void DEC_Reg_W(u8 rrr) {
	(*REG(RRR(rrr)))--;
}

void DIV(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	s16 *reg;
	s16 *ax=REG(AX);
	s16 *dx=REG(DX);
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo == 3) {
        reg=REG(RRR(mmm));
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

void HLT(){ exit(0);}

void IDIV(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16){
	u32 temp;
	s16 *reg;
	s16 *ax=REG(AX);
	s16 *dx=REG(DX);
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo == 3) {
        reg=REG(RRR(mmm));
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
	s16 *dx=REG(DX);
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo == 3) {
        reg=REG(RRR(mmm));
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
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
	u32 res;
	switch(oo){
		case 0:
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]+1;
			break;
		case 1:
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]+1;
			break;
		case 2:
			res=MEM[abs_addr];
			MEM[abs_addr]=MEM[abs_addr]+1;
			break;
		case 3:
			reg=REG(RRR(mmm));
			res=*reg;
			*reg=*reg+1;
			break;
	}
	(OVERFLOW_SUM(res,1)) ? SET_FLAG(OF) : CLR_FLAG(OF);
	FLAG_CHECK(res+1,0,1,1,1,1);
}

void INC_Reg_W(u8 rrr) {
	(*REG(RRR(rrr)))++;
}

void INT(u8 type) {
    u8 ah = GET_HI(*REG(AX));
    char last_read = '\0';
    // video interrupt
    if (type == 0x10) {
        switch (ah) {
            // set video mode
            case 0x00:
                break;
            // write graphics pixel
            case 0x0c:
                set_pixel(*REG(CX),*REG(DX),GET_LOW(*REG(AX)));
                break;
        }
    // keyboard interrupt
    } else if (type == 0x16) {
        switch (ah) {
            // read keyboard input
            case 0x00:
                SET_LOW(REG(AX), last_read);
                SET_HI(REG(AX), last_read);
                break;
            // return keyboard status
            case 0x01:
                SET_LOW(REG(AX), last_read);
                SET_HI(REG(AX), last_read);
	            FLAG_CHECK(last_read,1,1,1,1,1);
                break;
            case 0x02:
                break;
            case 0x03:
                break;
            case 0x05:
                break;
            case 0x10:
                break;
            case 0x11:
                break;
            case 0x12:
                break;
        }
    // DOS interrupt
    } else if (type == 0x21) {
        switch (ah) {
            // read character from stdin
            case 0x01:
                break;
            // write character to stdout
            case 0x02:
                // get character value from DL
                last_read = GET_LOW(*REG(DX));
                if (!show_parser_output) {
                    printf("%c", last_read);
                }
                break;
            // console input/output
            case 0x06:
                break;
            case 0x07:
                break;
            case 0x08:
                break;
            // write string to stdout
            case 0x09:
                // read characters as bytes from memory at DX until '$' is read
                for (int addr = *REG(DX); addr <= *REG(DX) + 100 && last_read != '$'; addr += 2) {
                    last_read = (MEM[addr] << 4) + MEM[addr + 1];
                    if (last_read != '$' && !show_parser_output) {
                        printf("%c", last_read);
                    }
                }
                if (!show_parser_output) {
                    printf("\n");
                }
                break;
            case 0x0a:
                break;
            case 0x0b:
                break;
            case 0x0c:
                break;
        }
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
	u32 abs_addr = calc_abs_addr(1, oo, mmm, imm8, imm16);
	*reg=MEM[abs_addr];
	*ds=MEM[abs_addr+2];	
}

void LES(u8 oo, u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg=REG(RRR(rrr));
	s16 *es=REG(ES);
	u32 abs_addr = calc_abs_addr(0, oo, mmm, imm8, imm16);
	*reg=MEM[abs_addr];
	*es=MEM[abs_addr+2];	
}

void LEA(u8 oo, u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg=REG(RRR(rrr));
	u32 abs_addr = calc_abs_addr(1, oo, mmm, imm8, imm16);
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

void LOOP(s8 imm8) {
    (*REG(CX))--;
    if (*REG(CX)) {
        JMP_Short(imm8);
    }
}

void LOOPZ(s8 imm8) {
    (*REG(CX))--;
    if (*REG(CX) && GET_FLAG(ZF)) {
        JMP_Short(imm8);
    }
}

void LOOPNZ(s8 imm8) {
    (*REG(CX))--;
    if (*REG(CX) && !GET_FLAG(ZF)) {
        JMP_Short(imm8);
    }
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
    s16 *reg;
    if (w) {
        reg = REG(RRR(rrr));
    } else {
        // AL, CL, DL, BL, AH, CH, DH, BH
        reg = REG(RRR(rrr % 4));
    }
	if (w) {
		*reg = imm16;
	} else {
        if (rrr <= 3) {
            SET_LOW(reg, imm8);
        } else {
            SET_HI(reg, imm8);
        }
	}
}

void MOV_Mem_Imm(u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
	s16 *reg;
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
	if(w){
		MEM[abs_addr]=imm_dat16;
	}
	else{
		MEM[abs_addr]=imm_dat8;
	}
	
}

void MOV_Reg_Reg(u8 w,u8 rrr,u8 mmm){
    if (w) {
        s16 *reg_a = REG(RRR(rrr));
        s16 *reg_b = REG(RRR(mmm));
        *reg_a = *reg_b;
    } else {
        // AL, CL, DL, BL, AH, CH, DH, BH
        s16 *reg_a = REG(RRR(rrr % 4));
        s16 *reg_b = REG(RRR(mmm % 4));
        if (rrr <= 3) {
            if (mmm <= 3) {
                SET_LOW(reg_a, GET_LOW(*reg_b));
            } else {
                SET_LOW(reg_a, GET_HI(*reg_b));
            }
        } else {
            if (mmm <= 3) {
                SET_HI(reg_a, GET_LOW(*reg_b));
            } else {
                SET_HI(reg_a, GET_HI(*reg_b));
            }
        }
    }
}

void MOV_Reg_Mem(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
	s16 *reg=REG(RRR(rrr));
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
	s16 *reg=REG(SSS(sss));
	u32 abs_addr = calc_abs_addr(0, oo, mmm, imm8, imm16);
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
	s16 *dx=REG(DX);
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo == 3) {
        reg=REG(RRR(mmm));
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
	s16 *reg;
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo == 3) {
        reg=REG(RRR(mmm));
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
	s16 *reg;
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo == 3) {
        reg=REG(RRR(mmm));
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
    func_RM(&or, d, w, oo, rrr, mmm, imm8, imm16);
}

void OR_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&or, w, imm8, imm16);
}

void OR_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&or, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}

void POP_Reg_W(u8 rrr){
	s16* reg=REG(RRR(rrr));
	*reg=MEM[ABS(*REG(SS),*REG(SP))];
	(*REG(SP))+=2;
}

void POP_Mem_W(u8 oo,u8 mmm,s8 imm8,s16 imm16){
	s16 *reg;
	u32 abs_addr = calc_abs_addr(1, oo, mmm, imm8, imm16);
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
	u32 abs_addr = calc_abs_addr(1, oo, mmm, imm8, imm16);
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

void JMP_Short(s8 offset){
	*(REG(IP)) += (offset << 1);
}
void JMP_Near(s16 offset){
	*(REG(IP)) += (offset << 1);
}
void JMP_Far(s8 lo_ip,s8 hi_ip,s8 lo_cs,s8 hi_cs){
	s16 offset=	((hi_ip << 8) | 0x00ff) & lo_ip;
	s16 new_cs=	((hi_cs << 8) | 0x00ff) & lo_cs;
	*REG(IP)=offset;
	*(REG(CS))=new_cs;
}
void JO(s8 offset) {
    if (GET_FLAG(OF)) {
        JMP_Short(offset);
    }
}
void JNO(s8 offset) {
    if (!GET_FLAG(OF)) {
        JMP_Short(offset);
    }
}
void JC(s8 offset) {
    if (GET_FLAG(CF)) {
        JMP_Short(offset);
    }
}
void JNC(s8 offset) {
    if (GET_FLAG(CF)) {
        JMP_Short(offset);
    }
}
void JE(s8 offset) {
    if (GET_FLAG(ZF)) {
        JMP_Short(offset);
    }
}
void JNE(s8 offset) {
    if (!GET_FLAG(CF)) {
        JMP_Short(offset);
    }
}
void JBE(s8 offset) {
    if (GET_FLAG(CF) || GET_FLAG(CF)) {
        JMP_Short(offset);
    }
}
void JA(s8 offset) {
    if (!GET_FLAG(CF) && !GET_FLAG(CF)) {
        JMP_Short(offset);
    }
}
void JS(s8 offset) {
    if (GET_FLAG(SF)) {
        JMP_Short(offset);
    }
}
void JNS(s8 offset) {
    if (!GET_FLAG(SF)) {
        JMP_Short(offset);
    }
}
void JP(s8 offset) {
    if (GET_FLAG(PF)) {
        JMP_Short(offset);
    }
}
void JNP(s8 offset) {
    if (!GET_FLAG(PF)) {
        JMP_Short(offset);
    }
}
void JL(s8 offset) {
    if (GET_FLAG(SF) != GET_FLAG(OF)) {
        JMP_Short(offset);
    }
}
void JGE(s8 offset) {
    if (GET_FLAG(SF) == GET_FLAG(OF)) {
        JMP_Short(offset);
    }
}
void JLE(s8 offset) {
    if (GET_FLAG(SF) != GET_FLAG(OF) || GET_FLAG(ZF)) {
        JMP_Short(offset);
    }
}
void JG(s8 offset) {
    if (!GET_FLAG(ZF) && GET_FLAG(SF) == GET_FLAG(OF)) {
        JMP_Short(offset);
    }
}
void JCXZ(s8 offset) {
    if (!*REG(CX)) {
        JMP_Short(offset);
    }
}

void NOP() {
}

void RET_Near(){
	*REG(SP)+=2;
	*REG(IP)=MEM[*REG(SP)];
}

void RET_Imm_Near(s16 imm){
	*REG(IP)=MEM[*REG(SP)];
	*REG(SP)+=imm;
}

void RET_Far(){
	*REG(SP)+=2;
	*REG(IP)=MEM[*REG(SP)];
	*REG(SP)+=2;
	*REG(CS)=MEM[*REG(SP)];
}
void RET_Imm_Far(s16 imm){
	*REG(IP)=MEM[*REG(SP)];
	*REG(SP)+=2;
	*REG(CS)=MEM[*REG(SP)];
	*REG(SP)+=imm;
}

void SAL(u8 places, u8 w, u8 oo, u8 mmm, s8 imm8, s16 imm16) {
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, -1);
    s16 *left_op_ptr;
    if (oo >= 0 && oo <= 2) {
        left_op_ptr = &MEM[abs_addr];
    } else {
        left_op_ptr = REG(RRR(mmm));
    }
    if (places == 1) {
        *REG(CF) = SIGN(*left_op_ptr);
    }
    *left_op_ptr <<= places;
}

void SAR(u8 places, u8 w, u8 oo, u8 mmm, s8 imm8, s16 imm16) {
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, -1);
    s16 *left_op_ptr;
    if (oo >= 0 && oo <= 2) {
        left_op_ptr = &MEM[abs_addr];
    } else {
        left_op_ptr = REG(RRR(mmm));
    }
    if (places == 1) {
        *REG(CF) = *left_op_ptr % 2;
    }
    *left_op_ptr >>= places;
}

void SHR(u8 places, u8 w, u8 oo, u8 mmm, s8 imm8, s16 imm16) {
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, -1);
    s16 *left_op_ptr;
    if (oo >= 0 && oo <= 2) {
        left_op_ptr = &MEM[abs_addr];
    } else {
        left_op_ptr = REG(RRR(mmm));
    }
    if (places == 1) {
        *REG(CF) = *left_op_ptr % 2;
    }
    *left_op_ptr = ((u16) (*left_op_ptr)) >> places;
}

void SBB_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
    func_RM(&sbb, d, w, oo, rrr, mmm, imm8, imm16);
}

void SBB_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&sbb, w, imm8, imm16);
}

void SBB_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&sbb, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}

void STC() {
    SET_FLAG(CF);
}
void STD() {
    SET_FLAG(DF);
}
void STI() {
    SET_FLAG(IF);
}

void SUB_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
    func_RM(&sub, d, w, oo, rrr, mmm, imm8, imm16);
}

void SUB_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&sub, w, imm8, imm16);
}

void SUB_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&sub, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}

void TEST_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
    func_RM(&test, d, w, oo, rrr, mmm, imm8, imm16);
}

void TEST_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&test, w, imm8, imm16);
}

void TEST_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&test, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}

void XCHG_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16) {
    s16 *left_op_ptr = REG(RRR(rrr)), *right_op_ptr;
	u32 abs_addr = calc_abs_addr(w, oo, mmm, imm8, imm16);
    if (oo >= 0 && oo <= 2) {
        right_op_ptr = &MEM[abs_addr];
    } else {
        right_op_ptr = REG(RRR(mmm));
    }
    s16 temp = *left_op_ptr;
    *left_op_ptr = *right_op_ptr;
    *right_op_ptr = temp;
}

void XCHG_Acc_W_Reg_W(u8 rrr) {
    s16 temp = *REG(RRR(rrr));
    *REG(RRR(rrr)) = *REG(AX);
    *REG(AX) = temp;
}

void XOR_RM(u8 d,u8 w,u8 oo,u8 rrr,u8 mmm,s8 imm8,s16 imm16){
    func_RM(&xor, d, w, oo, rrr, mmm, imm8, imm16);
}

void XOR_Acc_Imm(u8 w,s8 imm8,s16 imm16){
    func_Acc_Imm(&xor, w, imm8, imm16);
}

void XOR_RMI(u8 s,u8 w,u8 oo,u8 mmm,s8 imm8,s16 imm16,s8 imm_dat8,s16 imm_dat16){
    func_RMI(&xor, s, w, oo, mmm, imm8, imm16, imm_dat8, imm_dat16);
}


void CALL_Near(s16 offset){
	MEM[*REG(SP)]=*REG(IP);
	*REG(SP) -= 2;
	*(REG(IP)) += (offset << 1);
}
void CALL_Far(s8 lo_ip,s8 hi_ip,s8 lo_cs,s8 hi_cs){
	s16 offset=	((hi_ip << 8) | 0x00ff) & lo_ip;
	s16 new_cs=	((hi_cs << 8) | 0x00ff) & lo_cs;
	MEM[*REG(SP)]=*REG(IP);
	*REG(SP)-=2;
	*REG(IP)=offset;
	*(REG(CS))=new_cs;
}
