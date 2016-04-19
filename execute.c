#include "isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*func_M)(u8, u8, u8, s8, s16);
typedef void (*func_M16)(u8, u8, s8, s16);
typedef void (*func_RM)(u8, u8, u8, u8, u8, s8, s16);
typedef void (*func_RMI)(u8, u8, u8, u8, s8, s16, s8, s16);
typedef void (*func_Acc_Imm)(u8, s8, s16);
typedef void (*func_L)(u8, u8, u8, s8, s16);

char str_to_int(char* imm_str, short width) {
    char imm = 0;
    for (int i = 0; i < width; i++) {
        imm <<= 1;
        imm += imm_str[i] - '0'; 
    }
    return imm;
}

char parse_imm8(char* imm8_str) {
    char imm8 = 0;
    for (int i = 0; i < 8; i++) {
        imm8 <<= 1;
        imm8 += imm8_str[i] - '0'; 
    }
    return imm8;
}

short parse_imm16(char* imm) {
    short imm16 = 0;
    //Little-endian format
    for (int i = 8; i < 16; i++) {
        imm16 <<= 1;
        imm16 += imm[i] - '0'; 
    }
    for (int i = 0; i < 8; i++) {
        imm16 <<= 1;
        imm16 += imm[i] - '0'; 
    }
    return imm16;
}

func_M get_func_M(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1111111", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return &DEC;
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return &DIV;
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return &IDIV;
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &IMUL;
    }
    if (memcmp(opcode, "1111111", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &INC;
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return &MUL;
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &NEG;
    }
    return NULL;
}

func_M16 get_func_M16(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "10001111", 8) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &POP_Mem_W;
    }
    if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return &PUSH_Mem_W;
    }
    return NULL;
}

/*
func_M1 get_func_M1(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return &RCL;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &RCR;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &ROL;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return &ROR;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return &SAL;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return &SAR;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &SHR;
    }
    return NULL;
}
*/

func_RM get_func_RM(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "0001001", 7) == 0) {
        return &ADC_RM;
    }
    if (memcmp(opcode, "0000001", 7) == 0) {
        return &ADD_RM;
    }
    if (memcmp(opcode, "0010001", 7) == 0) {
        return &AND_RM;
    }
    if (memcmp(opcode, "0011101", 7) == 0) {
        return &CMP_RM;
    }
    if (memcmp(opcode, "0000101", 7) == 0) {
        return &OR_RM;
    }
    /*
    if (memcmp(opcode, "0001101", 7) == 0) {
        return &SBB_RM;
    }
    if (memcmp(opcode, "0010101", 7) == 0) {
        return &SUB_RM;
    }
    if (memcmp(opcode, "1000010", 7) == 0) {
        return &TEST_RM;
    }
    if (memcmp(opcode, "1000011", 7) == 0) {
        return &XCHG_RM;
    }
    if (memcmp(opcode, "0011001", 7) == 0) {
        return &XOR_RM;
    }
    */
    return NULL;
}

func_RM get_func_MR(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "0001000", 7) == 0) {
        return &ADC_RM;
    }
    if (memcmp(opcode, "0000000", 7) == 0) {
        return &ADD_RM;
    }
    if (memcmp(opcode, "0010000", 7) == 0) {
        return &AND_RM;
    }
    if (memcmp(opcode, "0011100", 7) == 0) {
        return &CMP_RM;
    }
    /*
    if (memcmp(opcode, "1000100", 7) == 0) {
        return &MOV_Mem_Reg;
    }
    */
    if (memcmp(opcode, "0000100", 7) == 0) {
        return &OR_RM;
    }
    /*
    if (memcmp(opcode, "0001100", 7) == 0) {
        return &SBB_RM;
    }
    if (memcmp(opcode, "0010100", 7) == 0) {
        return &SUB_RM;
    }
    */
    /*
    if (memcmp(opcode, "1000010", 7) == 0) {
        return &TEST_RM;
    }
    if (memcmp(opcode, "1000011", 7) == 0) {
        return &XCHG_RM";
    }
    */
    /*
    if (memcmp(opcode, "0011000", 7) == 0) {
        return &XOR_RM;
    }
    */
    return NULL;
}

func_Acc_Imm get_func_Acc_Imm(char* opcode, short width) {
    if (width < 8) {
        return NULL;
    }
    if (memcmp(opcode, "0001010", 7) == 0) {
        return &ADC_Acc_Imm;
    }
    if (memcmp(opcode, "0000010", 7) == 0) {
        return &ADD_Acc_Imm;
    }
    if (memcmp(opcode, "0010010", 7) == 0) {
        return &AND_Acc_Imm;
    }
    if (memcmp(opcode, "0011110", 7) == 0) {
        return &CMP_Acc_Imm;
    }
    if (memcmp(opcode, "0000110", 7) == 0) {
        return &OR_Acc_Imm;
    }
    /*
    if (memcmp(opcode, "0001110", 7) == 0) {
        return &SBB_Acc_Imm;
    }
    if (memcmp(opcode, "0010110", 7) == 0) {
        return &SUB_Acc_Imm;
    }
    if (memcmp(opcode, "1010100", 7) == 0) {
        return &TEST_Acc_Imm;
    }
    if (memcmp(opcode, "0011010", 7) == 0) {
        return &XOR_Acc_Imm;
    }
    */
    return NULL;
}

func_RMI get_func_RMI8(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return &ADC_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &ADD_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return &AND_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return &CMP_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return &OR_RMI;
    }
    /*
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &SBB_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &SUB_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return &XOR_RMI;
    }
    */
    return NULL;
}

func_RMI get_func_RMI(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return &ADC_RMI;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &ADD_RMI;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return &AND_RMI;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return &CMP_RMI;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return &OR_RMI;
    }
    /*
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &SBB_RMI;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &SUB_RMI;
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &TEST_RMI;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return &XOR_RMI;
    }
    if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &CALL_RMI;
    }
    if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &JMP_RMI;
    }
    */
    return NULL;
}

func_L get_func_L(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "11000101", 8) == 0) {
        return &LDS;
    }
    if (memcmp(opcode, "11000100", 8) == 0) {
        return &LES;
    }
    if (memcmp(opcode, "10001101", 8) == 0) {
        return &LEA;
    }
    return NULL;
}

char* get_name_M(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1111111", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return "DEC";
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return "DIV";
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return "IDIV";
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return "IMUL";
    }
    if (memcmp(opcode, "1111111", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "INC";
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return "MUL";
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return "NEG";
    }
    return NULL;
}

char* get_name_M16(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "10001111", 8) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "POP";
    }
    if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return "PUSH";
    }
    return NULL;
}

char* get_name_M1(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return "RCL";
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return "RCR";
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "ROL";
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return "ROR";
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return "SAL/SHL";
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return "SAR";
    }
    if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return "SHR";
    }
    return NULL;
}

char* get_name_M_CL(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return "RCL";
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return "RCR";
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "ROL";
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return "ROR";
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return "SAL/SHL";
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return "SAR";
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return "SHR";
    }
    return NULL;
}

char* get_name_RM(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "0001001", 7) == 0) {
        return "ADC";
    }
    if (memcmp(opcode, "0000001", 7) == 0) {
        return "ADD";
    }
    if (memcmp(opcode, "0010001", 7) == 0) {
        return "AND";
    }
    if (memcmp(opcode, "0011101", 7) == 0) {
        return "CMP";
    }
    if (memcmp(opcode, "1000101", 7) == 0) {
        return "MOV";
    }
    if (memcmp(opcode, "0000101", 7) == 0) {
        return "OR";
    }
    if (memcmp(opcode, "0001101", 7) == 0) {
        return "SBB";
    }
    if (memcmp(opcode, "0010101", 7) == 0) {
        return "SUB";
    }
    if (memcmp(opcode, "1000010", 7) == 0) {
        return "TEST";
    }
    if (memcmp(opcode, "1000011", 7) == 0) {
        return "XCHG";
    }
    if (memcmp(opcode, "0011001", 7) == 0) {
        return "XOR";
    }
    return NULL;
}

char* get_name_MR(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "0001000", 7) == 0) {
        return "ADC";
    }
    if (memcmp(opcode, "0000000", 7) == 0) {
        return "ADD";
    }
    if (memcmp(opcode, "0010000", 7) == 0) {
        return "AND";
    }
    if (memcmp(opcode, "0011100", 7) == 0) {
        return "CMP";
    }
    if (memcmp(opcode, "1000100", 7) == 0) {
        return "MOV";
    }
    if (memcmp(opcode, "0000100", 7) == 0) {
        return "OR";
    }
    if (memcmp(opcode, "0001100", 7) == 0) {
        return "SBB";
    }
    if (memcmp(opcode, "0010100", 7) == 0) {
        return "SUB";
    }
    /*
    if (memcmp(opcode, "1000010", 7) == 0) {
        return "TEST";
    }
    if (memcmp(opcode, "1000011", 7) == 0) {
        return "XCHG";
    }
    */
    if (memcmp(opcode, "0011000", 7) == 0) {
        return "XOR";
    }
    return NULL;
}

char* get_name_Acc_Imm(char* opcode, short width) {
    if (width < 8) {
        return NULL;
    }
    if (memcmp(opcode, "0001010", 7) == 0) {
        return "ADC";
    }
    if (memcmp(opcode, "0000010", 7) == 0) {
        return "ADD";
    }
    if (memcmp(opcode, "0010010", 7) == 0) {
        return "AND";
    }
    if (memcmp(opcode, "0011110", 7) == 0) {
        return "CMP";
    }
    if (memcmp(opcode, "0000110", 7) == 0) {
        return "OR";
    }
    if (memcmp(opcode, "0001110", 7) == 0) {
        return "SBB";
    }
    if (memcmp(opcode, "0010110", 7) == 0) {
        return "SUB";
    }
    if (memcmp(opcode, "1010100", 7) == 0) {
        return "TEST";
    }
    if (memcmp(opcode, "0011010", 7) == 0) {
        return "XOR";
    }
    return NULL;
}

char* get_name_RMI8(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return "ADC";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "ADD";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return "AND";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return "CMP";
    }
    if (width >= 16 && memcmp(opcode, "1100011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "MOV";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return "OR";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return "SBB";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return "SUB";
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return "XOR";
    }
    return NULL;
}

char* get_name_RMI(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return "ADC";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "ADD";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return "AND";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return "CMP";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return "OR";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return "SBB";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return "SUB";
    }
    if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return "TEST";
    }
    if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return "XOR";
    }
    if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return "CALL";
    }
    if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return "JMP";
    }
    return NULL;
}

char* get_name_L(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "11000101", 8) == 0) {
        return "LDS";
    }
    if (memcmp(opcode, "11000100", 8) == 0) {
        return "LES";
    }
    if (memcmp(opcode, "10001101", 8) == 0) {
        return "LEA";
    }
    return NULL;
}

char* get_mmm_name(char* mmm) {
    if (memcmp(mmm, "000", 3) == 0) {
        return "DS:[BX+SI]";
    } else if (memcmp(mmm, "001", 3) == 0) {
        return "DS:[BX+DI]";
    } else if (memcmp(mmm, "010", 3) == 0) {
        return "SS:[BP+SI]";
    } else if (memcmp(mmm, "011", 3) == 0) {
        return "SS:[BP+DI]";
    } else if (memcmp(mmm, "100", 3) == 0) {
        return "DS:[SI]";
    } else if (memcmp(mmm, "101", 3) == 0) {
        return "DS:[DI]";
    } else if (memcmp(mmm, "110", 3) == 0) {
        return "SS:[BP]";
    } else if (memcmp(mmm, "111", 3) == 0) {
        return "DS:[BX]";
    }
    return "Invalid";
}

char* get_rrr_name(char* rrr, char reg32, char w) {
    if (memcmp(rrr, "000", 3) == 0) {
        return reg32 ? "EAX" : w == '0' ? "AL" : "AX";
    } else if (memcmp(rrr, "001", 3) == 0) {
        return reg32 ? "ECX" : w == '0' ? "CL" : "CX";
    } else if (memcmp(rrr, "010", 3) == 0) {
        return reg32 ? "EDX" : w == '0' ? "DL" : "DX";
    } else if (memcmp(rrr, "011", 3) == 0) {
        return reg32 ? "EBX" : w == '0' ? "BL" : "BX";
    } else if (memcmp(rrr, "100", 3) == 0) {
        return reg32 ? "ESP" : w == '0' ? "AH" : "SP";
    } else if (memcmp(rrr, "101", 3) == 0) {
        return reg32 ? "EBP" : w == '0' ? "CH" : "BP";
    } else if (memcmp(rrr, "110", 3) == 0) {
        return reg32 ? "ESI" : w == '0' ? "DH" : "SI";
    } else if (memcmp(rrr, "111", 3) == 0) {
        return reg32 ? "EDI" : w == '0' ? "BH" : "DI";
    }
    return "Invalid";
}

char* get_sss_name(char* sss) {
    if (memcmp(sss, "000", 3) == 0) {
        return "ES";
    } else if (memcmp(sss, "001", 3) == 0) {
        return "CS";
    } else if (memcmp(sss, "010", 3) == 0) {
        return "SS";
    } else if (memcmp(sss, "011", 3) == 0) {
        return "DS";
    } else {
        return "Invalid";
    }
    return "Invalid";
}

char* get_cccc_name(char* cccc) {
    if (memcmp(cccc, "0000", 4) == 0) {
        return "O";
    } else if (memcmp(cccc, "0001", 4) == 0) {
        return "NO";
    } else if (memcmp(cccc, "0010", 4) == 0) {
        return "C/B/NAE";
    } else if (memcmp(cccc, "0011", 4) == 0) {
        return "NC/AE/NB";
    } else if (memcmp(cccc, "0100", 4) == 0) {
        return "E/Z";
    } else if (memcmp(cccc, "0101", 4) == 0) {
        return "NE/NZ";
    } else if (memcmp(cccc, "0110", 4) == 0) {
        return "BE/NA";
    } else if (memcmp(cccc, "0111", 4) == 0) {
        return "A/NBE";
    } else if (memcmp(cccc, "1000", 4) == 0) {
        return "S";
    } else if (memcmp(cccc, "1001", 4) == 0) {
        return "NS";
    } else if (memcmp(cccc, "1010", 4) == 0) {
        return "P/PE";
    } else if (memcmp(cccc, "1011", 4) == 0) {
        return "NP/PO";
    } else if (memcmp(cccc, "1100", 4) == 0) {
        return "L/NGE";
    } else if (memcmp(cccc, "1101", 4) == 0) {
        return "GE/NL";
    } else if (memcmp(cccc, "1110", 4) == 0) {
        return "LE/NG";
    } else if (memcmp(cccc, "1111", 4) == 0) {
        return "G/NLE";
    }
    return "Invalid";
}

// Return >0 if opcode matches the format of some instruction, 0 if not
short exec_instr(char* opcode, short width, FILE* fp) {
    short matches = 0;
    char reg32 = 0, w, oo[3], mmm[4], rrr[4], sss[4], cccc[5];
    char imm8 = -1;
    short imm16 = -1;
    oo[2] = '\0';
    mmm[3] = '\0';
    rrr[3] = '\0';
    sss[3] = '\0';
    cccc[4] = '\0';
    *oo = '\0';
    *mmm = '\0';
    *rrr = '\0';
    *sss = '\0';
    *cccc = '\0';
    printf("%s", opcode);
    char* instr_name = get_name_RM(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Reg,Mem", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Reg,Mem", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Reg,Mem", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg,Reg", instr_name);
        }
        // Execute the instruction 
        func_RM func = get_func_MR(opcode, width);
        func(opcode[6], w - '0', str_to_int(oo, 2), str_to_int(rrr, 3)
                , str_to_int(mmm, 3), imm8, imm16);
    }
    instr_name = get_name_MR(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Mem,Reg", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Mem,Reg", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Mem,Reg", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg,Reg", instr_name);
        }
        func_RM func = get_func_MR(opcode, width);
        func(opcode[6], w - '0', str_to_int(oo, 2), str_to_int(rrr, 3)
                , str_to_int(mmm, 3), imm8, imm16);
    }
    instr_name = get_name_Acc_Imm(opcode, width);
    if (instr_name) {
        w = opcode[7];
        matches++;
        func_Acc_Imm func = get_func_Acc_Imm(opcode, width);
        func(w - '0', imm8, imm16);
        printf("  %s Acc,Imm", instr_name);
    }
    instr_name = get_name_RMI8(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        char imm_dat8;
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            if (width == 24) {
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  %s Mem,Imm8", instr_name);
            }
        } else if (width >= 24 && memcmp(oo, "01", 2) == 0) {
            imm8 = parse_imm8(&opcode[16]);
            if (width == 32) {
                imm_dat8 = parse_imm8(&opcode[24]);
                matches++;
                printf("  %s Mem,Imm8", instr_name);
            }
        } else if (width >= 32 && (memcmp(oo, "10", 2) == 0
                || memcmp(oo, "00", 2) == 0)) {
            imm16 = parse_imm16(&opcode[16]);
            if (width == 40) {
                imm_dat8 = parse_imm8(&opcode[32]);
                matches++;
                printf("  %s Mem,Imm8", instr_name);
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            if (width == 24) {
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  %s Reg,Imm8", instr_name);
            }
        }
        func_RMI func = get_func_RMI8(opcode, width);
        func(opcode[6], w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8
                , imm16, imm_dat8, -1);
        printf("  %d", imm_dat8);
    }
    instr_name = get_name_RMI(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        char imm_dat8 = -1;
        short imm_dat16 = -1;
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            if (w == '0' && width == 24) {
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 32) {
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        } else if (width >= 24 && memcmp(oo, "01", 2) == 0) {
            imm8 = parse_imm8(&opcode[16]);
            if (w == '0' && width == 32) {
                imm_dat8 = parse_imm8(&opcode[24]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 40) {
                imm_dat16 = parse_imm16(&opcode[24]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        } else if (width >= 32 && (memcmp(oo, "10", 2) == 0
                || memcmp(oo, "00", 2) == 0)) {
            imm16 = parse_imm16(&opcode[16]);
            if (w == '0' && width == 40) {
                imm_dat8 = parse_imm8(&opcode[32]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 48) {
                imm_dat16 = parse_imm16(&opcode[32]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            if (w == '0' && width == 24) {
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  %s Reg,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 32) {
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                printf("  %s Reg,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        }
        func_RMI func = get_func_RMI8(opcode, width);
        func(opcode[6], w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8
                , imm16, imm_dat8, imm_dat16);
    }
    instr_name = get_name_M(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Mem", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Mem", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Mem", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg", instr_name);
        }
        func_M func = get_func_M(opcode, width);
        func(w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16);
    }
    instr_name = get_name_M16(opcode, width);
    if (instr_name) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s MemWord", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s MemWord", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s MemWord", instr_name);
        }/* else if (memcmp(oo, "11", 2) == 0) { // should never be called
            matches++;
            printf("  %s RegWord", instr_name);
        }*/
        func_M16 func = get_func_M16(opcode, width);
        func(str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16);
    }
    instr_name = get_name_M1(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Mem,1", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Mem,1", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Mem,1", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg,1", instr_name);
        }
    }
    instr_name = get_name_M_CL(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Mem,CL", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Mem,CL", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Mem,CL", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg,CL", instr_name);
        }
    }
    instr_name = get_name_L(opcode, width);
    if (instr_name) {
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Reg16,Mem32", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Reg16,Mem32", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Reg16,Mem32", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg16,Reg16", instr_name);
        }
        func_L func = get_func_L(opcode, width);
        func(str_to_int(oo, 2), str_to_int(rrr, 3), str_to_int(mmm, 3), imm8, imm16);
    }
    if (memcmp(opcode, "00110111", 8) == 0) {
        matches++;
        AAA();
        printf("  AAA");
    }
    if (width == 16 && memcmp(opcode, "1101010100001010", 16) == 0) {
        matches++;
        AAD();
        printf("  AAD");
    }
    if (width == 16 && memcmp(opcode, "1101010000001010", 16) == 0) {
        matches++;
        AAM();
        printf("  AAM");
    }
    if (memcmp(opcode, "00111111", 8) == 0) {
        matches++;
        AAS();
        printf("  AAS");
    }
    if (memcmp(opcode, "10011000", 8) == 0) {
        matches++;
        CBW();
        printf("  CBW");
    }
    if (memcmp(opcode, "11111000", 8) == 0) {
        matches++;
        CLC();
        printf("  CLC");
    }
    if (memcmp(opcode, "11111100", 8) == 0) {
        matches++;
        CLD();
        printf("  CLD");
    }
    if (memcmp(opcode, "11111010", 8) == 0) {
        matches++;
        CLI();
        printf("  CLI");
    }
    if (memcmp(opcode, "11110101", 8) == 0) {
        matches++;
        CMC();
        printf("  CMC");
    }
    if (memcmp(opcode, "10100110", 8) == 0) {
        matches++;
        CMPSB();
        printf("  CMPSB");
    }
    if (memcmp(opcode, "10100111", 8) == 0) {
        matches++;
        CMPSW();
        printf("  CMPSW");
    }
    if (memcmp(opcode, "10011001", 8) == 0) {
        matches++;
        CWD();
        printf("  CWD");
    }
    if (memcmp(opcode, "00100111", 8) == 0) {
        matches++;
        DAA();
        printf("  DAA");
    }
    if (memcmp(opcode, "00101111", 8) == 0) {
        matches++;
        DAS();
        printf("  DAS");
    }
    if (memcmp(opcode, "01001", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        //DEC_RegWord(rrr);
        printf("  DEC RegWord");
    }
    if (memcmp(opcode, "11110100", 8) == 0) {
        matches++;
        HLT();
        printf("  HLT");
    }
    if (memcmp(opcode, "1110110", 7) == 0) {
        w = opcode[7];
        matches++;
        printf("  IN Acc,DX");
    }
    if (memcmp(opcode, "01000", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        //INC_RegWord(rrr);
        printf("  INC RegWord");
    }
    if (memcmp(opcode, "11001100", 8) == 0) {
        matches++;
        //INT(3);
        printf("  INT 3");
    }
    if (memcmp(opcode, "11001110", 8) == 0) {
        matches++;
        //INTO();
        printf("  INTO");
    }
    if (memcmp(opcode, "11001111", 8) == 0) {
        matches++;
        IRET();
        printf("  IRET");
    }
    if (memcmp(opcode, "10011111", 8) == 0) {
        matches++;
        LAHF();
        printf("  LAHF");
    }
    if (memcmp(opcode, "10101100", 8) == 0) {
        matches++;
        LODSB();
        printf("  LODSB");
    }
    if (memcmp(opcode, "10101101", 8) == 0) {
        matches++;
        LODSW();
        printf("  LODSW");
    }
    if (width == 24 && memcmp(opcode, "1010001", 7) == 0) {
        w = opcode[7];
        if (width == 24) {
            imm16 = parse_imm16(&opcode[8]);
            matches++;
            MOV_MOFS_Acc(opcode[6], w, imm8, imm16);
            printf("  MOV MemOfs,Acc");
        }
    }
    if (width >= 16 && memcmp(opcode, "1010000", 7) == 0) {
        w = opcode[7];
        if (width == 24) {
            imm16 = parse_imm16(&opcode[8]);
            matches++;
            MOV_MOFS_Acc(opcode[6], w, imm8, imm16);
            printf("  MOV Acc,MemOfs");
        }
    }
    if (width >= 16 && memcmp(opcode, "1011", 4) == 0) {
        w = opcode[4];
        memcpy(rrr, &opcode[5], 3);
        if (w == '0') {
            imm8 = parse_imm8(&opcode[8]);
            matches++;
            printf("  MOV Reg,Imm");
        } else if (width == 24) {
            imm16 = parse_imm16(&opcode[8]);
            matches++;
            printf("  MOV Reg,Imm");
        }
        MOV_Reg_Imm(w - '0', str_to_int(rrr, 3), imm8, imm16);
    }
    if (width >= 16 && memcmp(opcode, "1000101", 7) == 0) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  %s Reg,Mem", instr_name);
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  %s Reg,Mem", instr_name);
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  %s Reg,Mem", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  %s Reg,Reg", instr_name);
        }
        MOV_Reg_Reg( - '0', str_to_int(rrr, 3), str_to_int(mmm, 3));
    }
    if (width >= 16 && memcmp(opcode, "1100011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        char imm_dat8 = -1;
        short imm_dat16 = -1;
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            if (w == '0' && width == 24) {
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 32) {
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        } else if (width >= 24 && memcmp(oo, "01", 2) == 0) {
            imm8 = parse_imm8(&opcode[16]);
            if (w == '0' && width == 32) {
                imm_dat8 = parse_imm8(&opcode[24]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 40) {
                imm_dat16 = parse_imm16(&opcode[24]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        } else if (width >= 32 && (memcmp(oo, "10", 2) == 0
                || memcmp(oo, "00", 2) == 0)) {
            imm16 = parse_imm16(&opcode[16]);
            if (w == '0' && width == 40) {
                imm_dat8 = parse_imm8(&opcode[32]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 48) {
                imm_dat16 = parse_imm16(&opcode[32]);
                matches++;
                printf("  %s Mem,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            if (w == '0' && width == 24) {
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  %s Reg,Imm", instr_name);
                printf("  %d", imm_dat8);
            } else if (width == 32) {
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                printf("  %s Reg,Imm", instr_name);
                printf("  %d", imm_dat16);
            }
        }
        MOV_Mem_Imm(w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16,
                imm_dat8, imm_dat16);
    }
    if (width >= 16 && memcmp(opcode, "10001100", 8) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(sss, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
                matches++;
                printf("  MOV Mem16,Seg");
                MOV_Mem_Seg(opcode[6], str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                    || (memcmp(oo, "01", 2) == 0))) {
                imm8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  MOV Mem16,Seg");
                MOV_Mem_Seg(opcode[6], str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
                imm16 = parse_imm16(&opcode[16]);
                matches++;
                printf("  MOV Mem16,Seg");
                MOV_Mem_Seg(opcode[6], str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (memcmp(oo, "11", 2) == 0) {
                matches++;
                printf("  MOV Reg16,Seg");
                MOV_Reg_Seg(opcode[6], w - '0', str_to_int(sss, 3), str_to_int(mmm, 3));
            }
        }
    }
    if (width >= 16 && memcmp(opcode, "10001110", 8) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(sss, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
                matches++;
                printf("  MOV Seg,Mem16");
                MOV_Mem_Seg(opcode[6], str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                    || (memcmp(oo, "01", 2) == 0))) {
                imm8 = parse_imm8(&opcode[16]);
                matches++;
                printf("  MOV Seg,Mem16");
                MOV_Mem_Seg(opcode[6], str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
                imm16 = parse_imm16(&opcode[16]);
                matches++;
                printf("  MOV Seg,Mem16");
                MOV_Mem_Seg(opcode[6], str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (memcmp(oo, "11", 2) == 0) {
                matches++;
                printf("  MOV Seg,Reg16");
                MOV_Reg_Seg(opcode[6], w - '0', str_to_int(sss, 3), str_to_int(mmm, 3));
            }
        }
    }
    if (memcmp(opcode, "10100100", 8) == 0) {
        matches++;
        MOVSB();
        printf("  MOVSB");
    }
    if (memcmp(opcode, "10100101", 8) == 0) {
        matches++;
        MOVSW();
        printf("  MOVSW");
    }
    /*
    if (memcmp(opcode, "10010000", 8) == 0) {
        matches++;
        printf("  NOP");
    }
    */
    if (memcmp(opcode, "1110111", 7) == 0) {
        w = opcode[7];
        matches++;
        printf("  OUT DX,Acc");
    }
    if (memcmp(opcode, "01011", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        PUSH_Reg_W(str_to_int(rrr, 3));
        printf("  POP RegWord");
    }
    if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "111", 3) == 0) {
        memcpy(sss, &opcode[2], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            matches++;
            POP_Seg(str_to_int(sss, 3));
            printf("  POP SegOld");
        }
    }
    if (memcmp(opcode, "10011101", 8) == 0) {
        matches++;
        POPF();
        printf("  POPF");
    }
    if (memcmp(opcode, "01010", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        PUSH_Reg_W(str_to_int(rrr, 3));
        printf("  PUSH RegWord");
    }
    if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "110", 3) == 0) {
        memcpy(sss, &opcode[2], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            matches++;
            PUSH_Seg(str_to_int(sss, 3));
            printf("  PUSH SegOld");
        }
    }
    if (memcmp(opcode, "10011100", 8) == 0) {
        matches++;
        PUSHF();
        printf("  PUSHF");
    }
    if (memcmp(opcode, "11000011", 8) == 0) {
        matches++;
        RET_Near();
        printf("  RET NEAR");
    }
    if (width >= 24 && memcmp(opcode, "11000010", 8) == 0) {
        imm16 = parse_imm16(&opcode[8]);
        matches++;
        RET_Imm_Near(imm16);
        printf("  RET imm NEAR");
    }
    if (memcmp(opcode, "11001011", 8) == 0) {
        matches++;
        RET_Far();
        printf("  RET FAR");
    }
    if (width >= 24 && memcmp(opcode, "11001010", 8) == 0) {
        imm16 = parse_imm16(&opcode[24]);
        matches++;
        RET_Imm_Far(imm16);
        printf("  RET imm FAR");
    }
    if (memcmp(opcode, "10011110", 8) == 0) {
        matches++;
        //SAHF();
        printf("  SAHF");
    }
    if (memcmp(opcode, "10101110", 8) == 0) {
        matches++;
        //SCASB();
        printf("  SCASB");
    }
    if (memcmp(opcode, "10101111", 8) == 0) {
        matches++;
        //SCASW();
        printf("  SCASW");
    }
    if (memcmp(opcode, "11111001", 8) == 0) {
        matches++;
        //STC();
        printf("  STC");
    }
    if (memcmp(opcode, "11111101", 8) == 0) {
        matches++;
        //STD();
        printf("  STD");
    }
    if (memcmp(opcode, "11111011", 8) == 0) {
        matches++;
        //STI();
        printf("  STI");
    }
    if (memcmp(opcode, "10101010", 8) == 0) {
        matches++;
        //STOSB();
        printf("  STOSB");
    }
    if (memcmp(opcode, "10101011", 8) == 0) {
        matches++;
        //STOSW();
        printf("  STOSW");
    }
    if (memcmp(opcode, "10011011", 8) == 0) {
        matches++;
        //WAIT();
        printf("  WAIT/FWAIT");
    }
    if (memcmp(opcode, "10010", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        //XCHG_AccWord_RegWord(rrr);
        printf("  XCHG AccWord/RegWord, RegWord/AccWord");
    }
    if (memcmp(opcode, "11010111", 8) == 0) {
        matches++;
        //XLAT();
        printf("  XLAT");
    }
    if (memcmp(opcode, "0011010", 7) == 0) {
        w = opcode[7];
        matches++;
        //XOR_Acc_Imm(w);
        printf("  XOR");
    }
    if (memcmp(opcode, "11110000", 8) == 0) {
        matches++;
        //LOCK();
        printf("  LOCK");
    }
    if (memcmp(opcode, "11110011", 8) == 0) {
        matches++;
        //REP();
        printf("  REP/REPE/REPZ");
    }
    if (memcmp(opcode, "11110010", 8) == 0) {
        matches++;
        //REPNE();
        printf("  REPNE/REPNZ");
    }
    if (width >= 16 && memcmp(opcode, "1110010", 7) == 0) {
        w = opcode[7];
        matches++;
        //IN_Acc_Imm8();
        printf("  IN Acc,Imm8");
    }
    if (width >= 16 && memcmp(opcode, "11001101", 8) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        //INT(imm8);
        printf("  INT Imm8");
    }
    if (width >= 16 && memcmp(opcode, "1110011", 7) == 0) {
        w = opcode[7];
        matches++;
        printf("  OUT Imm8,Acc");
    }
    if (width >= 16 && memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  CALL MemNear");
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  CALL MemNear");
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  CALL MemNear");
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  CALL RegWord");
        }
    }
    if (width == 24 && memcmp(opcode, "11101000", 8) == 0) {
        imm16 = parse_imm16(&opcode[8]);
        matches++;
        CALL_Near(parse_imm8(opcode), parse_imm8(&opcode[8]));
        printf("  CALL Near");
    }
    if (width == 40 && memcmp(opcode, "10011010", 8) == 0) {
        imm16 = parse_imm16(&opcode[8]);
        short offset = parse_imm16(&opcode[24]);
        matches++;
        CALL_Far(parse_imm8(opcode), parse_imm8(&opcode[8]), parse_imm8(&opcode[16])
                , parse_imm8(&opcode[24]));
        printf("  CALL Far");
    }
    if (width >= 16 && memcmp(opcode, "0111", 4) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        memcpy(cccc, &opcode[4], 4);
        matches++;
        if (memcmp(cccc, "0000", 4) == 0) {
            //JO();
            //return "O";
        } else if (memcmp(cccc, "0001", 4) == 0) {
            //JNO();
            //return "NO";
        } else if (memcmp(cccc, "0010", 4) == 0) {
            //JC();
            //return "C/B/NAE";
        } else if (memcmp(cccc, "0011", 4) == 0) {
            //JNC();
            //return "NC/AE/NB";
        } else if (memcmp(cccc, "0100", 4) == 0) {
            //JE();
            //return "E/Z";
        } else if (memcmp(cccc, "0101", 4) == 0) {
            //JNE();
            //return "NE/NZ";
        } else if (memcmp(cccc, "0110", 4) == 0) {
            //JBE();
            //return "BE/NA";
        } else if (memcmp(cccc, "0111", 4) == 0) {
            //JA();
            //return "A/NBE";
        } else if (memcmp(cccc, "1000", 4) == 0) {
            //JS();
            //return "S";
        } else if (memcmp(cccc, "1001", 4) == 0) {
            //JNS();
            //return "NS";
        } else if (memcmp(cccc, "1010", 4) == 0) {
            //JP();
            //return "P/PE";
        } else if (memcmp(cccc, "1011", 4) == 0) {
            //JNP();
            //return "NP/PO";
        } else if (memcmp(cccc, "1100", 4) == 0) {
            //JL();
            //return "L/NGE";
        } else if (memcmp(cccc, "1101", 4) == 0) {
            //JGE();
            //return "GE/NL";
        } else if (memcmp(cccc, "1110", 4) == 0) {
            //JLE();
            //return "LE/NG";
        } else if (memcmp(cccc, "1111", 4) == 0) {
            //JG();
            //return "G/NLE";
        }
        printf("  Jcc Short");
    }
    if (width >= 16 && memcmp(opcode, "11100011", 8) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        //JCXZ();
        printf("  JCXZ/JCXE/JECXZ/JECXE");
    }
    if (width >= 16 && memcmp(opcode, "11101011", 8) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        JMP_Short(imm8);
        printf("  JMP Short");
    }
    if (width == 24 && memcmp(opcode, "11101001", 8) == 0) {
        imm16 = parse_imm16(&opcode[8]);
        matches++;
        JMP_Near(parse_imm8(opcode), parse_imm8(&opcode[8]));
        printf("  JMP Near");
    }
    if (width == 40 && memcmp(opcode, "11101010", 8) == 0) {
        imm16 = parse_imm16(&opcode[8]);
        short offset = parse_imm16(&opcode[24]);
        matches++;
        JMP_Far(parse_imm8(opcode), parse_imm8(&opcode[8]), parse_imm8(&opcode[16])
                , parse_imm8(&opcode[24]));
        printf("  JMP Far");
    }
    if (width >= 16 && memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            printf("  JMP MemNear");
        } else if (width == 24 && (memcmp(oo, "00", 2) == 0
                || (memcmp(oo, "01", 2) == 0))) {
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            printf("  JMP MemNear");
        } else if (width == 32 && memcmp(oo, "10", 2) == 0) {
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            printf("  JMP MemNear");
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            printf("  JMP RegWord");
        }
    }
    if (width >= 16 && memcmp(opcode, "11100010", 8) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        //LOOP();
        printf("  LOOP");
    }
    if (width >= 16 && memcmp(opcode, "11100001", 8) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        //LOOPZ();
        printf("  LOOPZ/LOOPE");
    }
    if (width >= 16 && memcmp(opcode, "11100000", 8) == 0) {
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        //LOOPNZ();
        printf("  LOOPNZ/LOOPNE");
    }
    if (width >= 16 && memcmp(opcode, "1101111011010101", 16) == 0) {
        matches++;
        //FUCOMPP();
        printf("  FUCOMPP");
    }
    if (memcmp(opcode, "00101110", 8) == 0) {
        matches++;
        printf("  CS");
    }
    if (memcmp(opcode, "00111110", 8) == 0) {
        matches++;
        printf("  DS");
    }
    if (memcmp(opcode, "00100110", 8) == 0) {
        matches++;
        printf("  ES");
    }
    if (memcmp(opcode, "01100100", 8) == 0) {
        matches++;
        printf("  FS");
    }
    if (!matches && width >= 48) {
        printf("  Error: No match");
    } else if (matches > 1) {
        printf("  Error: Ambiguity in parsing");
    }
    if (*mmm != '\0') {
        printf("  mmm: %s", memcmp(oo, "11", 2) ? get_mmm_name(mmm) : get_rrr_name(mmm, reg32, w), mmm);
    }
    if (*rrr != '\0') {
        printf("  rrr: %s", get_rrr_name(rrr, reg32, w), rrr);
    }
    if (*sss != '\0') {
        printf("  sss: %s", get_sss_name(sss), sss);
    }
    if (*cccc != '\0') {
        printf("  cccc: %s", get_cccc_name(cccc), cccc);
    }
    printf("  %d %d", imm8, imm16);
    return matches;
}

int exec(char* fname) {
	FILE *fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "Can't open input file %s\n", fname);
		exit(1);
	}
    /*
	int bytes_read;
	size_t nbytes = 100;
	char *buf = (char*) malloc(nbytes + 1);
	bytes_read = getline(&buf, &nbytes, fp);
    */
    char opcode[49] = {0};
    short pos = 0;
    // The currently scanned 4-letter hexadecimal word
	char hexword[5];
	while (fscanf(fp, "%s", hexword) != EOF) {
        for (int i = 0; i < strlen(hexword); i++) {
            printf("%c ", hexword[i]);
            // Buffer holding next four binary digits, initialized to 0
            short val;
            if (hexword[i] >= 'A' && hexword[i] <= 'F') {
                val = hexword[i] - 'A' + 10;
            } else if (hexword[i] >= 'a' && hexword[i] <= 'f') {
                val = hexword[i] - 'a' + 10;
            } else {
                val = hexword[i] - '0';
            }
            for (int j = 3; j >= 0; j--) {
                if (val & (1 << j)) {
                    opcode[pos] = '1';
                } else {
                    opcode[pos] = '0';
                }
                printf("%c", opcode[pos]);
                pos++;
                // Check if we have a complete instruction
                if (pos > 0 && !(pos % 8)) {
                    printf(" ");
                    if (exec_instr(opcode, pos, fp) || pos == 48) {
                        memset(opcode, 0, sizeof(opcode));
                        pos = 0;
                    }
                }
            }
            printf("\n");
        }
	}
	fclose(fp);
	return 0;
}
