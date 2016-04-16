#ifndef ISA_H
#define ISA_H
#include "types.h"

#define MEMSIZE ( 0x1<< 20)
#define BITMASK(i) (0x1 << i)

/**"Data structures"**/

//Conditional Flags
#define CF (0x1 & (FLAGS)) //Carry Flag
#define AF (0x1 & (FLAGS)) //Aux Flag
#define PF (0x1 & (FLAGS)) //Parity Flag
#define ZF (0x1 & (FLAGS)) //Zero Flag
#define SF (0x1 & (FLAGS)) //Sign Flag
#define OF (0x1 & (FLAGS)) //Overflow Flag
//Control Flags
#define TF (0x1 & (FLAGS)) //Trap Flag
#define IF (0x1 & (FLAGS)) //Interrupt Flag
#define DF (0x1 & (FLAGS)) //Direction Flag


s16 MEM[MEMSIZE];
s16 FLAGS=0x0;


#endif