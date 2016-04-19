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


void ADC_RM(u8,u8,u8,u8,u8,s8,s16);
void ADC_Acc_Imm(u8,s8,s16);
void ADC_RMI(u8,u8,u8,u8,s8,s16,s8,s16);

void ADD_RM(u8,u8,u8,u8,u8,s8,s16);
void ADD_Acc_Imm(u8,s8,s16);
void ADD_RMI(u8,u8,u8,u8,s8,s16,s8,s16);

void AND_RM(u8,u8,u8,u8,u8,s8,s16);
void AND_Acc_Imm(u8,s8,s16);
void AND_RMI(u8,u8,u8,u8,s8,s16,s8,s16);

void CBW();
void CLC();
void CLD();
void CLI();
void CMC();

void CMP_RM(u8,u8,u8,u8,u8,s8,s16);
void CMP_Acc_Imm(u8,s8,s16);
void CMP_RMI(u8,u8,u8,u8,s8,s16,s8,s16);

void CMPSB();
void CMPSW();
void CWD();
void DAA();
void DAS();

void DEC(u8,u8,u8,s8,s16);

void DIV(u8,u8,u8,s8,s16);

void HLT(); //TODO

void IDIV(u8,u8,u8,s8,s16);


#endif