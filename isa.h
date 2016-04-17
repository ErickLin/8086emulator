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
u16 SP;

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


void CALL(); //Implement THIS ********** once Push and pop are up

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
void DIV_Reg(s16*); //DIV not clear! mplement THIS ********** 
void DIV_Mem(u32); //THIS
void HLT(); //THIS figure this out too
void IDIV_Reg(); //THIS
void IDIV_Mem(); //THIS
void IMUL_Reg(); //THIS
void IMUL_Mem(); //THIS
void IN(); //THIS
void INC_Reg(s16*);
void INC_Mem(u32);
void INT(u32); //THIS
void INTO();
void IRET(); //THIS




#endif