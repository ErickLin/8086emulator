#ifndef ISA_H
#define ISA_H
#include "types.h"

#define MEMSIZE ( 0x1<< 20)
#define SEGSIZE ( 0x1 << 16)
#define BITMASK(i) (0x1 << i)
#define NIBBLE(data) (data & 0xf)
#define SIGN(data) (data & 0x8000)
#define CARRY(data) (data & 0x10000)
#define ADJUST(data) ((data >> 4) & 0x000f)
#define TC(data) (~(data) + 1) //2's complement
#define ABS(a,b) ((a*16) + b)


/**"Data structures"**/

#define CF 0
#define AF 4
#define PF 2
#define ZF 6
#define SF 7
#define OF 11
#define TF 8
#define IF 9
#define DF 10

#define AX 0
#define BX 1
#define CX 2
#define DX 3
#define SI 4
#define DI 5
#define BP 6
#define SP 7

#define CS 8
#define DS 9
#define ES 10
#define SS 11

#define IP 12
#define FLAGS 13


s16 MEM[MEMSIZE];

extern const char *REG_STR[14];

//Registers

typedef struct reg_file{
	//(General Purpose)
	s16 AX_;
	s16 BX_;
	s16 CX_;
	s16 DX_;
	u16 SI_;
	u16 DI_;
	u16 BP_;
	u16 SP_;

	//(Segment Registers)
	u16 CS_;
	u16 DS_;
	u16 ES_;
	u16 SS_;

	//Special Purpose
	u16 IP_;
	u16 FLAGS_;
} reg_file_t;

void SET_FLAG(u32);
u32 GET_FLAG(u32);
void TOGGLE_FLAG(u32);
void CLR_FLAG(u32);

void SET_HI(s16*,u8);
u16 GET_HI(s16);
void SET_LOW(s16*,u8);
u16 GET_LOW(s16);
void CLR_NIB(s16*,u16);
s16* REG(u8);
u8 PARITY(s16);
u8 OVERFLOW_SUM(u16,u16);

//Flag checks and sets used a ton
//Must check overflow yourself in function where you call this
void FLAG_CHECK(u32,u8,u8,u8,u8,u8);

u32 MMM(u8);
u8 RRR(u8);

//ISA implementation

void AAA();
void AAD();
void AAM();
void AAS();

void ADC(u8,u8,u8,u8,u8,s16);
void ADC_Mem(u8,u8,u8);
void ADC_Mem_8b(u8,u8,u8,s8);
void ADC_Mem_16b(u8,u8,u8,s16);
void ADC_Reg_8b(u8,u8,u8);
void ADC_Reg_16b(u8,u8,u8);

/*
void ADD_Reg_Mem(s16*,u32);
void ADD_Mem_Reg(u32,s16*);
void ADD_Reg_Reg(s16*,s16*);
void ADD_Mem_Imm(u32,s16);
void ADD_Reg_Imm(s16*,s16);

void AND_Reg_Mem(s16*,u32);
void AND_Mem_Reg(u32,s16*);
void AND_Reg_Reg(s16*,s16*);
void AND_Mem_Imm(u32,s16);
void AND_Reg_Imm(s16*,s16);


void CALL(u32); 

void CBW();

void CLC();

void CLI();

void CMC();

void CMP_Reg_Mem(s16*,u32);
void CMP_Mem_Reg(u32,s16*);
void CMP_Reg_Reg(s16*,s16*);
void CMP_Mem_Imm(u32,s16);
void CMP_Reg_Imm(s16*,s16);

void CMPSB();
void CMPSW();
void CWD();
void DAA();
void DAS();
void DEC_Reg(s16*);
void DEC_Mem(u32);
void DIV_Reg_8b(s16*);
void DIV_Mem_8b(u32); 
void DIV_Reg_16b(s16*); 
void DIV_Mem_16b(u32); 
void HLT(); //TODO figure out too
void IDIV_Reg_8b(s16*);
void IDIV_Mem_8b(u32); 
void IDIV_Reg_16b(s16*); 
void IDIV_Mem_16b(u32); 
void IMUL_Reg_8b(s16*); 
void IMUL_Mem_8b(u32);
void IMUL_Reg_16b(s16*);
void IMUL_Mem_16b(u32); 
void IN(); //TODO Might not even need to support this
void INC_Reg(s16*);
void INC_Mem(u32);
void INT(u32); //TODO save this for later when we do allll the interrupts
void INTO();
void IRET(); 
//All jumps we need to look at for CS:IP offset when using labels
void JA(u32);
void JAE(u32);
void JB(u32);
void JBE(u32);
void JC(u32);
void JCXZ(u32);
void JE(u32);
void JG(u32);
void JGE(u32);
void JL(u32);
void JLE(u32);
void JMP(u32);
void JNA(u32);
void JNAE(u32);
void JNB(u32);
void JNBE(u32);
void JNC(u32);
void JNE(u32);
void JNG(u32);
void JNGE(u32);
void JNL(u32);
void JNLE(u32);
void JNO(u32);
void JNP(u32);
void JNS(u32);
void JNZ(u32);
void JO(u32);
void JP(u32);
void JPE(u32);
void JPO(u32);
void JS(u32);
void JZ(u32);
void LAHF();
void LDS_Reg_Mem(s16*,u32);
void LEA_Mem_Reg(s16*,u32);
void LES(s16*,u32);
void LODSB();
void LODSW();
void LOOP(u32);
void LOOPE(u32);
void LOOPNE(u32);
void LOOPNZ(u32); //Same as LOOPNE? Am i hallucinating?

//Need to add checking accordin to spec
void MOV_Reg_Mem(s16*,u32);
void MOV_Mem_Reg(u32,s16*);
void MOV_Reg_Reg(s16*,s16*);
void MOV_Mem_Imm(u32,s16);
void MOV_Reg_Imm(s16*,s16);
void MOV_SReg_Mem(s16*,u32);
void MOV_Mem_SReg(u32,s16*);
void MOV_Reg_SReg(s16*,s16*);
void MOV_SReg_Reg(s16*,s16*);

void MOVSB();
void MOVSW();

void MUL_Reg_8b(s16*);
void MUL_Mem_8b(u32);
void MUL_Reg_16b(s16*);
void MUL_Mem_16b(u32);

void NEG_Reg(s16*);
void NEG_Mem(u32);

void NOP();

void OR_Reg_Mem(s16*,u32);
void OR_Mem_Reg(u32,s16*);
void OR_Reg_Reg(s16*,s16*);
void OR_Mem_Imm(u32,s16);
void OR_Reg_Imm(s16*,s16);

void OUT(); //Eh

void POP_Reg_Me();
*/



#endif