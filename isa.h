#ifndef ISA_H
#define ISA_H
#include "types.h"

#define MEMSIZE ( 0x1<< 20)
#define BITMASK(i) (0x1 << i)
#define NIBBLE(data) (data & 0xf)
#define SIGN(data) (data & 0x8000)
#define CARRY(data) (data & 0x10000)
#define ADJUST(data) ((data >> 4) & 0x000f)
#define TC(data) (~(data) + 1) //2's complement


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


s16 MEM[MEMSIZE];

//Registers
//(General Purpose)
s16 AX;
s16 BX;
s16 CX;
s16 DX;
u16 SI;
u16 DI;
u16 BP;
u16 SP_; //Name conflict with something in SDL package

//(Segment Registers)
u16 CS;
u16 DS;
u16 ES;
u16 SS;

//Special Purpose
u16 IP;
u16 FLAGS;

void SET_FLAG(u32);
u32 GET_FLAG(u32);
void TOGGLE_FLAG(u32);
void CLR_FLAG(u32);

void SET_HI(u16*,u8);
u16 GET_HI(u16);
void SET_LOW(u16*,u8);
u16 GET_LOW(u16);
void CLR_NIB(u16*,u16);
u8 PARITY(s16);
u8 OVERFLOW_SUM(u16,u16);

//ISA implementation

void AAA();
void AAD();
void AAM();
void AAS();

void ADC_Reg_Mem(s16*,u32);
void ADC_Mem_Reg(u32,s16*);
void ADC_Reg_Reg(s16*,s16*);
void ADC_Mem_Imm(u32,s16);
void ADC_Reg_Imm(s16*,s16);

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


#endif