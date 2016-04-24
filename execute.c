#include "debug.h"
#include "isa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*func_M)(u8, u8, u8, s8, s16);
typedef void (*func_M16)(u8, u8, s8, s16);
typedef void (*func_M1)(u8, u8, u8, u8, s8, s16);
typedef void (*func_MCL)(u8, u8, u8, u8, s8, s16);
typedef void (*func_RM)(u8, u8, u8, u8, u8, s8, s16);
typedef void (*func_RMI)(u8, u8, u8, u8, s8, s16, s8, s16);
typedef void (*func_Acc_Imm)(u8, s8, s16);
typedef void (*func_L)(u8, u8, u8, s8, s16);
typedef void (*func_Jcc)(s8);

s16 hex_char_to_s16(char word) {
    short val;
    if (word >= 'A' && word <= 'F') {
        val = word - 'A' + 10;
    } else if (word >= 'a' && word <= 'f') {
        val = word - 'a' + 10;
    } else {
        val = word - '0';
    }
    return val;
}

char s16_to_hex_char(s16 val) {
    if (val <= 9) {
        return val + '0';
    } else {
        return val + 'A' - 10;
    }
}

char str_to_int(char* imm_str, short width) {
    char imm = 0;
    for (int i = 0; i < width; i++) {
        imm <<= 1;
        imm += imm_str[i] - '0'; 
    }
    return imm;
}

char parse_imm8(char* imm_str) {
    return str_to_int(imm_str, 8);
}

short parse_imm16(char* imm_str) {
    short imm16 = 0;
    //Little-endian format
    for (int i = 8; i < 16; i++) {
        imm16 <<= 1;
        imm16 += imm_str[i] - '0'; 
    }
    for (int i = 0; i < 8; i++) {
        imm16 <<= 1;
        imm16 += imm_str[i] - '0'; 
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

func_M1 get_func_M1(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    /*
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
    */
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

func_MCL get_func_MCL(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    /*
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        return &RCL;
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &RCR;
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        return &ROL;
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
        return &ROR;
    }
    */
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        return &SAL;
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
        return &SAR;
    }
    if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &SHR;
    }
    return NULL;
}

func_RM get_func_RM(char* opcode, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(opcode, "000100", 6) == 0) {
        return &ADC_RM;
    }
    if (memcmp(opcode, "000000", 6) == 0) {
        return &ADD_RM;
    }
    if (memcmp(opcode, "001000", 6) == 0) {
        return &AND_RM;
    }
    if (memcmp(opcode, "001110", 6) == 0) {
        return &CMP_RM;
    }
    if (memcmp(opcode, "000010", 6) == 0) {
        return &OR_RM;
    }
    if (memcmp(opcode, "000110", 6) == 0) {
        return &SBB_RM;
    }
    if (memcmp(opcode, "001010", 6) == 0) {
        return &SUB_RM;
    }
    if (memcmp(opcode, "100001", 6) == 0) {
        return &TEST_RM;
    }
    if (memcmp(opcode, "100001", 6) == 0) {
        return &XCHG_RM;
    }
    if (memcmp(opcode, "001100", 6) == 0) {
        return &XOR_RM;
    }
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
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
        return &SBB_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
        return &SUB_RMI;
    }
    if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
        return &XOR_RMI;
    }
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

func_Jcc get_func_Jcc(char* cccc, short width) {
    if (width < 16) {
        return NULL;
    }
    if (memcmp(cccc, "0000", 4) == 0) {
        return &JO;
    } else if (memcmp(cccc, "0001", 4) == 0) {
        return &JNO;
    } else if (memcmp(cccc, "0010", 4) == 0) {
        return &JC;
    } else if (memcmp(cccc, "0011", 4) == 0) {
        return &JNC;
    } else if (memcmp(cccc, "0100", 4) == 0) {
        return &JE;
    } else if (memcmp(cccc, "0101", 4) == 0) {
        return &JNE;
    } else if (memcmp(cccc, "0110", 4) == 0) {
        return &JBE;
    } else if (memcmp(cccc, "0111", 4) == 0) {
        return &JA;
    } else if (memcmp(cccc, "1000", 4) == 0) {
        return &JS;
    } else if (memcmp(cccc, "1001", 4) == 0) {
        return &JNS;
    } else if (memcmp(cccc, "1010", 4) == 0) {
        return &JP;
    } else if (memcmp(cccc, "1011", 4) == 0) {
        return &JNP;
    } else if (memcmp(cccc, "1100", 4) == 0) {
        return &JL;
    } else if (memcmp(cccc, "1101", 4) == 0) {
        return &JGE;
    } else if (memcmp(cccc, "1110", 4) == 0) {
        return &JLE;
    } else if (memcmp(cccc, "1111", 4) == 0) {
        return &JG;
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
    if (memcmp(opcode, "000100", 6) == 0) {
        return "ADC";
    }
    if (memcmp(opcode, "000000", 6) == 0) {
        return "ADD";
    }
    if (memcmp(opcode, "001000", 6) == 0) {
        return "AND";
    }
    if (memcmp(opcode, "001110", 6) == 0) {
        return "CMP";
    }
    if (memcmp(opcode, "000010", 6) == 0) {
        return "OR";
    }
    if (memcmp(opcode, "000110", 6) == 0) {
        return "SBB";
    }
    if (memcmp(opcode, "001010", 6) == 0) {
        return "SUB";
    }
    if (memcmp(opcode, "100001", 6) == 0) {
        return "TEST";
    }
    if (memcmp(opcode, "100001", 6) == 0) {
        return "XCHG";
    }
    if (memcmp(opcode, "001100", 6) == 0) {
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

// Returns the type of instruction executed if opcode exists, NULL if not
uintptr_t exec_instr(char* opcode, short width, FILE* fp, short* imm_buf) {
    uintptr_t instr = NULL;
    short matches = 0;
    char reg32 = 0, d = '\0', w = '\0', oo[3], mmm[4], rrr[4], sss[4], cccc[5];
    char imm_type = 0, imm_dat_type = 0; //0 = no immediate, 1 = has imm8, 2 = has imm16
    char imm8 = -1, imm_dat8 = -1;
    short imm16 = -1, imm_dat16 = -1;
    u8 ah = -1;
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
    char full_instr_name[100];
    char* instr_name = get_name_RM(opcode, width);
    if (instr_name) {
        d = opcode[6];
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            if (d == '0') {
                sprintf(full_instr_name, "%s Mem,Reg", instr_name);
            } else {
                sprintf(full_instr_name, "%s Reg,Mem", instr_name);
            }
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            if (d == '0') {
                sprintf(full_instr_name, "%s Mem,Reg", instr_name);
            } else {
                sprintf(full_instr_name, "%s Reg,Mem", instr_name);
            }
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            if (d == '0') {
                sprintf(full_instr_name, "%s Mem,Reg", instr_name);
            } else {
                sprintf(full_instr_name, "%s Reg,Mem", instr_name);
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            sprintf(full_instr_name, "%s Reg,Reg", instr_name);
        }
        if (matches == 1) {
            // Execute the instruction 
            func_RM func = get_func_RM(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(d - '0', w - '0', str_to_int(oo, 2), str_to_int(rrr, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            }
        }
    }
    instr_name = get_name_Acc_Imm(opcode, width);
    if (instr_name) {
        w = opcode[7];
        if (w == '0' && width == 16) {
            imm_type = w - '0' + 1;
            imm8 = parse_imm8(&opcode[8]);
            matches++;
        } else if (w == '1' && width == 24) {
            imm_type = w - '0' + 1;
            imm16 = parse_imm16(&opcode[8]);
            matches++;
        }
        if (matches >= 1) {
            func_Acc_Imm func = get_func_Acc_Imm(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(w - '0', imm8, imm16);
            }
            sprintf(full_instr_name, "%s Acc,Imm", instr_name);
        }
    }
    instr_name = get_name_RMI8(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            if (width == 24) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm8", instr_name);
            }
        } else if (width >= 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            if (width == 32) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[24]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm8", instr_name);
            }
        } else if (width >= 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            if (width == 40) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[32]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm8", instr_name);
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            if (width == 24) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "%s Reg,Imm8", instr_name);
            }
        }
        if (matches == 1) {
            func_RMI func = get_func_RMI8(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                // first argument is s bit, which indicates size of imm_dat
                func(opcode[6] - '0', w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8
                        , imm16, imm_dat8, -1);
            }
        }
    }
    instr_name = get_name_RMI(opcode, width);
    if (instr_name) {
        d = opcode[6];
        char s = d;
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            if (s == '1' && width == 24) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm", instr_name);
            } else if (width == 32) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm", instr_name);
            }
        } else if (width >= 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            if (s == '1' && width == 32) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[24]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm", instr_name);
            } else if (width == 40) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[24]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm", instr_name);
            }
        } else if (width >= 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            if (s == '1' && width == 40) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[32]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm", instr_name);
            } else if (width == 48) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[32]);
                matches++;
                sprintf(full_instr_name, "%s Mem,Imm", instr_name);
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            if (s == '1' && width == 24) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "%s Reg,Imm", instr_name);
            } else if (width == 32) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "%s Reg,Imm", instr_name);
            }
        }
        if (matches == 1) {
            func_RMI func = get_func_RMI(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(s - '0', w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8
                        , imm16, imm_dat8, imm_dat16);
            }
        }
    }
    instr_name = get_name_M(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            sprintf(full_instr_name, "%s Mem", instr_name);
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Mem", instr_name);
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Mem", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            sprintf(full_instr_name, "%s Reg", instr_name);
        }
        if (matches == 1) {
            func_M func = get_func_M(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16);
            }
        }
    }
    instr_name = get_name_M16(opcode, width);
    if (instr_name) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            sprintf(full_instr_name, "%s MemWord", instr_name);
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s MemWord", instr_name);
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s MemWord", instr_name);
        }/* else if (memcmp(oo, "11", 2) == 0) { // should never be called
            matches++;
            sprintf(full_instr_name, "%s RegWord", instr_name);
        }*/
        if (matches == 1) {
            func_M16 func = get_func_M16(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16);
            }
        }
    }
    instr_name = get_name_M1(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            sprintf(full_instr_name, "%s Mem,1", instr_name);
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Mem,1", instr_name);
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Mem,1", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            sprintf(full_instr_name, "%s Reg,1", instr_name);
        }
        if (matches == 1) {
            func_M1 func = get_func_M1(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(1, w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16);
            }
        }
    }
    instr_name = get_name_M_CL(opcode, width);
    if (instr_name) {
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            sprintf(full_instr_name, "%s Mem,CL", instr_name);
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Mem,CL", instr_name);
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Mem,CL", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            sprintf(full_instr_name, "%s Reg,CL", instr_name);
        }
        if (matches == 1) {
            func_MCL func = get_func_MCL(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(GET_LOW(*REG(CX)), w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16);
            }
        }
    }
    instr_name = get_name_L(opcode, width);
    if (instr_name) {
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            sprintf(full_instr_name, "%s Reg16,Mem32", instr_name);
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Reg16,Mem32", instr_name);
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            sprintf(full_instr_name, "%s Reg16,Mem32", instr_name);
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            sprintf(full_instr_name, "%s Reg16,Reg16", instr_name);
        }
        if (matches == 1) {
            func_L func = get_func_L(opcode, width);
            if (func) {
                instr = (uintptr_t) func;
                func(str_to_int(oo, 2), str_to_int(rrr, 3), str_to_int(mmm, 3), imm8, imm16);
            }
        }
    }
    if (memcmp(opcode, "00110111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &AAA;
        AAA();
        sprintf(full_instr_name, "AAA");
    }
    if (width == 16 && memcmp(opcode, "1101010100001010", 16) == 0) {
        matches++;
        instr = (uintptr_t) &AAD;
        AAD();
        sprintf(full_instr_name, "AAD");
    }
    if (width == 16 && memcmp(opcode, "1101010000001010", 16) == 0) {
        matches++;
        instr = (uintptr_t) &AAM;
        AAM();
        sprintf(full_instr_name, "AAM");
    }
    if (memcmp(opcode, "00111111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &AAS;
        AAS();
        sprintf(full_instr_name, "AAS");
    }
    if (memcmp(opcode, "10011000", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CBW;
        CBW();
        sprintf(full_instr_name, "CBW");
    }
    if (memcmp(opcode, "11111000", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CLC;
        CLC();
        sprintf(full_instr_name, "CLC");
    }
    if (memcmp(opcode, "11111100", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CLD;
        CLD();
        sprintf(full_instr_name, "CLD");
    }
    if (memcmp(opcode, "11111010", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CLI;
        CLI();
        sprintf(full_instr_name, "CLI");
    }
    if (memcmp(opcode, "11110101", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CMC;
        CMC();
        sprintf(full_instr_name, "CMC");
    }
    if (memcmp(opcode, "10100110", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CMPSB;
        CMPSB();
        sprintf(full_instr_name, "CMPSB");
    }
    if (memcmp(opcode, "10100111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CMPSW;
        CMPSW();
        sprintf(full_instr_name, "CMPSW");
    }
    if (memcmp(opcode, "10011001", 8) == 0) {
        matches++;
        instr = (uintptr_t) &CWD;
        CWD();
        sprintf(full_instr_name, "CWD");
    }
    if (memcmp(opcode, "00100111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &DAA;
        DAA();
        sprintf(full_instr_name, "DAA");
    }
    if (memcmp(opcode, "00101111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &DAS;
        DAS();
        sprintf(full_instr_name, "DAS");
    }
    if (memcmp(opcode, "01001", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        instr = (uintptr_t) &DEC_Reg_W;
        DEC_Reg_W(str_to_int(rrr, 3));
        sprintf(full_instr_name, "DEC RegWord");
    }
    if (memcmp(opcode, "11110100", 8) == 0) {
        matches++;
        instr = (uintptr_t) &HLT;
        HLT();
        sprintf(full_instr_name, "HLT");
    }
    if (memcmp(opcode, "1110110", 7) == 0) {
        w = opcode[7];
        matches++;
        //instr = (uintptr_t) &IN;
        sprintf(full_instr_name, "IN Acc,DX");
    }
    if (memcmp(opcode, "01000", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        instr = (uintptr_t) &INC_Reg_W;
        INC_Reg_W(str_to_int(rrr, 3));
        sprintf(full_instr_name, "INC RegWord");
    }
    if (memcmp(opcode, "11001100", 8) == 0) {
        ah = GET_HI(*REG(AX));
        matches++;
        instr = (uintptr_t) &INT;
        INT(3);
        sprintf(full_instr_name, "INT 3");
    }
    if (memcmp(opcode, "11001110", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &INTO;
        //INTO();
        sprintf(full_instr_name, "INTO");
    }
    if (memcmp(opcode, "11001111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &IRET;
        IRET();
        sprintf(full_instr_name, "IRET");
    }
    if (memcmp(opcode, "10011111", 8) == 0) {
        matches++;
        instr = (uintptr_t) &LAHF;
        LAHF();
        sprintf(full_instr_name, "LAHF");
    }
    if (memcmp(opcode, "10101100", 8) == 0) {
        matches++;
        instr = (uintptr_t) &LODSB;
        LODSB();
        sprintf(full_instr_name, "LODSB");
    }
    if (memcmp(opcode, "10101101", 8) == 0) {
        matches++;
        instr = (uintptr_t) &LODSW;
        LODSW();
        sprintf(full_instr_name, "LODSW");
    }
    if (width == 24 && memcmp(opcode, "1010001", 7) == 0) {
        w = opcode[7];
        if (width == 24) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[8]);
            matches++;
            instr = (uintptr_t) &MOV_MOFS_Acc;
            MOV_MOFS_Acc(opcode[6] - '0', w, imm8, imm16);
            sprintf(full_instr_name, "MOV MemOfs,Acc");
        }
    }
    if (width >= 16 && memcmp(opcode, "1010000", 7) == 0) {
        w = opcode[7];
        if (width == 24) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[8]);
            matches++;
            instr = (uintptr_t) &MOV_MOFS_Acc;
            MOV_MOFS_Acc(opcode[6] - '0', w, imm8, imm16);
            sprintf(full_instr_name, "MOV Acc,MemOfs");
        }
    }
    if (width >= 16 && memcmp(opcode, "1011", 4) == 0) {
        w = opcode[4];
        memcpy(rrr, &opcode[5], 3);
        if (w == '0') {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[8]);
            matches++;
            sprintf(full_instr_name, "MOV Reg,Imm");
        } else if (width == 24) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[8]);
            matches++;
            sprintf(full_instr_name, "MOV Reg,Imm");
        }
        if (matches == 1) {
            instr = (uintptr_t) &MOV_Reg_Imm;
            MOV_Reg_Imm(w - '0', str_to_int(rrr, 3), imm8, imm16);
        }
    }
    if (width >= 16 && memcmp(opcode, "100010", 6) == 0) {
        d = opcode[6];
        w = opcode[7];
        memcpy(oo, &opcode[8], 2);
        memcpy(rrr, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            // Displacement does not follow operation if mmm != 110
            matches++;
            instr = (uintptr_t) &MOV_Reg_Mem;
            MOV_Reg_Mem(d - '0', w - '0', str_to_int(oo, 2), str_to_int(rrr, 3), str_to_int(mmm, 3), imm8, imm16);
            if (d == '0') {
                sprintf(full_instr_name, "MOV Mem,Reg");
            } else {
                sprintf(full_instr_name, "MOV Reg,Mem");
            }
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            instr = (uintptr_t) &MOV_Reg_Mem;
            MOV_Reg_Mem(d - '0', w - '0', str_to_int(oo, 2), str_to_int(rrr, 3), str_to_int(mmm, 3), imm8, imm16);
            if (d == '0') {
                sprintf(full_instr_name, "MOV Mem,Reg");
            } else {
                sprintf(full_instr_name, "MOV Reg,Mem");
            }
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            instr = (uintptr_t) &MOV_Reg_Mem;
            MOV_Reg_Mem(d - '0', w - '0', str_to_int(oo, 2), str_to_int(rrr, 3), str_to_int(mmm, 3), imm8, imm16);
            if (d == '0') {
                sprintf(full_instr_name, "MOV Mem,Reg");
            } else {
                sprintf(full_instr_name, "MOV Reg,Mem");
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            instr = (uintptr_t) &MOV_Reg_Reg;
            MOV_Reg_Reg(w - '0', str_to_int(rrr, 3), str_to_int(mmm, 3));
            sprintf(full_instr_name, "MOV Reg,Reg");
        }
    }
    if (width >= 16 && memcmp(opcode, "1100011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
        d = opcode[6];
        char s = d;
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            if (s == '1' && width == 24) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Mem,Imm");
            } else if (width == 32) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Mem,Imm");
            }
        } else if (width >= 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            if (s == '1' && width == 32) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[24]);
                matches++;
                sprintf(full_instr_name, "MOV Mem,Imm");
            } else if (width == 40) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[24]);
                matches++;
                sprintf(full_instr_name, "MOV Mem,Imm");
            }
        } else if (width >= 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            if (s == '1' && width == 40) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[32]);
                matches++;
                sprintf(full_instr_name, "MOV Mem,Imm");
            } else if (width == 48) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[32]);
                matches++;
                sprintf(full_instr_name, "MOV Mem,Imm");
            }
        } else if (memcmp(oo, "11", 2) == 0) {
            if (s == '1' && width == 24) {
                imm_dat_type = 1;
                imm_dat8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Reg,Imm");
            } else if (width == 32) {
                imm_dat_type = 2;
                imm_dat16 = parse_imm16(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Reg,Imm");
            }
        }
        if (matches == 1) {
            instr = (uintptr_t) &MOV_Mem_Imm;
            MOV_Mem_Imm(w - '0', str_to_int(oo, 2), str_to_int(mmm, 3), imm8, imm16,
                    imm_dat8, imm_dat16);
        }
    }
    if (width >= 16 && memcmp(opcode, "10001100", 8) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(sss, &opcode[10], 3);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
                matches++;
                sprintf(full_instr_name, "MOV Mem16,Seg");
                instr = (uintptr_t) &MOV_Mem_Seg;
                MOV_Mem_Seg(opcode[6] - '0', str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                    || (w == '0' && memcmp(oo, "00", 2) == 0))) {
                imm_type = 1;
                imm8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Mem16,Seg");
                instr = (uintptr_t) &MOV_Mem_Seg;
                MOV_Mem_Seg(opcode[6] - '0', str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 32 && (memcmp(oo, "10", 2) == 0 
                    || (w == '1' && memcmp(oo, "00", 2) == 0))) {
                imm_type = 2;
                imm16 = parse_imm16(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Mem16,Seg");
                instr = (uintptr_t) &MOV_Mem_Seg;
                MOV_Mem_Seg(opcode[6] - '0', str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (memcmp(oo, "11", 2) == 0) {
                matches++;
                sprintf(full_instr_name, "MOV Reg16,Seg");
                instr = (uintptr_t) &MOV_Reg_Seg;
                MOV_Reg_Seg(opcode[6] - '0', opcode[7] - '0', str_to_int(sss, 3), str_to_int(mmm, 3));
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
                sprintf(full_instr_name, "MOV Seg,Mem16");
                instr = (uintptr_t) &MOV_Mem_Seg;
                MOV_Mem_Seg(opcode[6] - '0', str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                    || (w == '0' && memcmp(oo, "00", 2) == 0))) {
                imm_type = 1;
                imm8 = parse_imm8(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Seg,Mem16");
                instr = (uintptr_t) &MOV_Mem_Seg;
                MOV_Mem_Seg(opcode[6] - '0', str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                    || (w == '1' && memcmp(oo, "00", 2) == 0))) {
                imm_type = 2;
                imm16 = parse_imm16(&opcode[16]);
                matches++;
                sprintf(full_instr_name, "MOV Seg,Mem16");
                instr = (uintptr_t) &MOV_Mem_Seg;
                MOV_Mem_Seg(opcode[6] - '0', str_to_int(oo, 2), str_to_int(sss, 3)
                        , str_to_int(mmm, 3), imm8, imm16);
            } else if (memcmp(oo, "11", 2) == 0) {
                matches++;
                sprintf(full_instr_name, "MOV Seg,Reg16");
                instr = (uintptr_t) &MOV_Reg_Seg;
                MOV_Reg_Seg(opcode[6] - '0', opcode[7] - '0', str_to_int(sss, 3), str_to_int(mmm, 3));
            }
        }
    }
    if (memcmp(opcode, "10100100", 8) == 0) {
        matches++;
        instr = (uintptr_t) &MOVSB;
        MOVSB();
        sprintf(full_instr_name, "MOVSB");
    }
    if (memcmp(opcode, "10100101", 8) == 0) {
        matches++;
        instr = (uintptr_t) &MOVSW;
        MOVSW();
        sprintf(full_instr_name, "MOVSW");
    }
    // Conflicts with another opcode
    /*
    if (memcmp(opcode, "10010000", 8) == 0) {
        matches++;
        instr = (uintptr_t) &NOP;
        sprintf(full_instr_name, "NOP");
    }
    */
    if (memcmp(opcode, "1110111", 7) == 0) {
        w = opcode[7];
        matches++;
        //instr = (uintptr_t) &OUT;
        sprintf(full_instr_name, "OUT DX,Acc");
    }
    if (memcmp(opcode, "01011", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        instr = (uintptr_t) &PUSH_Reg_W;
        PUSH_Reg_W(str_to_int(rrr, 3));
        sprintf(full_instr_name, "POP RegWord");
    }
    if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "111", 3) == 0) {
        memcpy(sss, &opcode[2], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            matches++;
            instr = (uintptr_t) &POP_Seg;
            POP_Seg(str_to_int(sss, 3));
            sprintf(full_instr_name, "POP SegOld");
        }
    }
    if (memcmp(opcode, "10011101", 8) == 0) {
        matches++;
        instr = (uintptr_t) &POPF;
        POPF();
        sprintf(full_instr_name, "POPF");
    }
    if (memcmp(opcode, "01010", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        instr = (uintptr_t) &PUSH_Reg_W;
        PUSH_Reg_W(str_to_int(rrr, 3));
        sprintf(full_instr_name, "PUSH RegWord");
    }
    if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "110", 3) == 0) {
        memcpy(sss, &opcode[2], 3);
        if (memcmp(get_sss_name(sss), "Invalid", 7)) {
            matches++;
            instr = (uintptr_t) &PUSH_Seg;
            PUSH_Seg(str_to_int(sss, 3));
            sprintf(full_instr_name, "PUSH SegOld");
        }
    }
    if (memcmp(opcode, "10011100", 8) == 0) {
        matches++;
        instr = (uintptr_t) &PUSHF;
        PUSHF();
        sprintf(full_instr_name, "PUSHF");
    }
    if (memcmp(opcode, "11000011", 8) == 0) {
        matches++;
        instr = (uintptr_t) &RET_Near;
        RET_Near();
        sprintf(full_instr_name, "RET NEAR");
    }
    if (width >= 24 && memcmp(opcode, "11000010", 8) == 0) {
        imm_type = 2;
        imm16 = parse_imm16(&opcode[8]);
        matches++;
        instr = (uintptr_t) &RET_Imm_Near;
        RET_Imm_Near(imm16);
        sprintf(full_instr_name, "RET imm NEAR");
    }
    if (memcmp(opcode, "11001011", 8) == 0) {
        matches++;
        instr = (uintptr_t) &RET_Far;
        RET_Far();
        sprintf(full_instr_name, "RET FAR");
    }
    if (width >= 24 && memcmp(opcode, "11001010", 8) == 0) {
        imm_type = 2;
        imm16 = parse_imm16(&opcode[24]);
        matches++;
        instr = (uintptr_t) &RET_Imm_Far;
        RET_Imm_Far(imm16);
        sprintf(full_instr_name, "RET imm FAR");
    }
    if (memcmp(opcode, "10011110", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &SAHF;
        //SAHF();
        sprintf(full_instr_name, "SAHF");
    }
    if (memcmp(opcode, "10101110", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &SCASB;
        //SCASB();
        sprintf(full_instr_name, "SCASB");
    }
    if (memcmp(opcode, "10101111", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &SCASW;
        //SCASW();
        sprintf(full_instr_name, "SCASW");
    }
    if (memcmp(opcode, "11111001", 8) == 0) {
        matches++;
        instr = (uintptr_t) &STC;
        STC();
        sprintf(full_instr_name, "STC");
    }
    if (memcmp(opcode, "11111101", 8) == 0) {
        matches++;
        instr = (uintptr_t) &STD;
        STD();
        sprintf(full_instr_name, "STD");
    }
    if (memcmp(opcode, "11111011", 8) == 0) {
        matches++;
        instr = (uintptr_t) &STI;
        STI();
        sprintf(full_instr_name, "STI");
    }
    if (memcmp(opcode, "10101010", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &STOSB;
        //STOSB();
        sprintf(full_instr_name, "STOSB");
    }
    if (memcmp(opcode, "10101011", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &STOSW;
        //STOSW();
        sprintf(full_instr_name, "STOSW");
    }
    if (memcmp(opcode, "10011011", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &WAIT;
        //WAIT();
        sprintf(full_instr_name, "WAIT/FWAIT");
    }
    if (memcmp(opcode, "10010", 5) == 0) {
        memcpy(rrr, &opcode[5], 3);
        matches++;
        instr = (uintptr_t) &XCHG_Acc_W_Reg_W;
        XCHG_Acc_W_Reg_W(str_to_int(rrr, 3));
        sprintf(full_instr_name, "XCHG AccWord/RegWord, RegWord/AccWord");
    }
    if (memcmp(opcode, "11010111", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &XLAT;
        //XLAT();
        sprintf(full_instr_name, "XLAT");
    }
    if (memcmp(opcode, "11110000", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &LOCK;
        //LOCK();
        sprintf(full_instr_name, "LOCK");
    }
    if (memcmp(opcode, "11110011", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &REP;
        //REP();
        sprintf(full_instr_name, "REP/REPE/REPZ");
    }
    if (memcmp(opcode, "11110010", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &REPNE;
        //REPNE();
        sprintf(full_instr_name, "REPNE/REPNZ");
    }
    if (width >= 16 && memcmp(opcode, "1110010", 7) == 0) {
        w = opcode[7];
        matches++;
        //instr = (uintptr_t) &IN_Acc_Imm8;
        //IN_Acc_Imm8();
        sprintf(full_instr_name, "IN Acc,Imm8");
    }
    if (width >= 16 && memcmp(opcode, "11001101", 8) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        ah = GET_HI(*REG(AX));
        matches++;
        instr = (uintptr_t) &INT;
        INT(imm8);
        sprintf(full_instr_name, "INT Imm8");
    }
    if (width >= 16 && memcmp(opcode, "1110011", 7) == 0) {
        w = opcode[7];
        matches++;
        //instr = (uintptr_t) &OUT_Imm8_Acc;
        //OUT_Imm8_Acc();
        sprintf(full_instr_name, "OUT Imm8,Acc");
    }
    if (width >= 16 && memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            //instr = (uintptr_t) &CALL_MemNear;
            sprintf(full_instr_name, "CALL MemNear");
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            //instr = (uintptr_t) &CALL_MemNear;
            sprintf(full_instr_name, "CALL MemNear");
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            //instr = (uintptr_t) &CALL_MemNear;
            sprintf(full_instr_name, "CALL MemNear");
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            //instr = (uintptr_t) &CALL_RegWord;
            sprintf(full_instr_name, "CALL RegWord");
        }
    }
    if (width == 24 && memcmp(opcode, "11101000", 8) == 0) {
        imm_type = 2;
        imm16 = parse_imm16(&opcode[8]);
        matches++;
        instr = (uintptr_t) &CALL_Near;
        CALL_Near(imm16);
        sprintf(full_instr_name, "CALL Near");
    }
    if (width == 40 && memcmp(opcode, "10011010", 8) == 0) {
        imm_type = 2;
        imm16 = parse_imm16(&opcode[8]);
        short offset = parse_imm16(&opcode[24]);
        matches++;
        instr = (uintptr_t) &CALL_Far;
        CALL_Far(parse_imm8(opcode), parse_imm8(&opcode[8]), parse_imm8(&opcode[16])
                , parse_imm8(&opcode[24]));
        sprintf(full_instr_name, "CALL Far");
    }
    if (width >= 16 && memcmp(opcode, "0111", 4) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        memcpy(cccc, &opcode[4], 4);
        matches++;
        func_Jcc func = get_func_Jcc(cccc, width);
        if (func) {
            instr = (uintptr_t) func;
            func(imm8);
        }
        sprintf(full_instr_name, "Jcc Short");
    }
    if (width >= 16 && memcmp(opcode, "11100011", 8) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        instr = (uintptr_t) &JCXZ;
        JCXZ(imm8);
        sprintf(full_instr_name, "JCXZ/JCXE/JECXZ/JECXE");
    }
    if (width >= 16 && memcmp(opcode, "11101011", 8) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        instr = (uintptr_t) &JMP_Short;
        JMP_Short(imm8);
        sprintf(full_instr_name, "JMP Short");
    }
    if (width == 24 && memcmp(opcode, "11101001", 8) == 0) {
        imm_type = 2;
        imm16 = parse_imm16(&opcode[8]);
        matches++;
        instr = (uintptr_t) &JMP_Near;
        JMP_Near(imm16);
        sprintf(full_instr_name, "JMP Near");
    }
    if (width == 40 && memcmp(opcode, "11101010", 8) == 0) {
        imm_type = 2;
        imm16 = parse_imm16(&opcode[8]);
        short offset = parse_imm16(&opcode[24]);
        matches++;
        instr = (uintptr_t) &JMP_Far;
        JMP_Far(parse_imm8(opcode), parse_imm8(&opcode[8]), parse_imm8(&opcode[16])
                , parse_imm8(&opcode[24]));
        sprintf(full_instr_name, "JMP Far");
    }
    if (width >= 16 && memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
        memcpy(oo, &opcode[8], 2);
        memcpy(mmm, &opcode[13], 3);
        if (memcmp(oo, "00", 2) == 0 && memcmp(mmm, "110", 3)) {
            matches++;
            //instr = (uintptr_t) &JMP_MemNear;
            sprintf(full_instr_name, "JMP MemNear");
        } else if (width == 24 && (memcmp(oo, "01", 2) == 0
                || (w == '0' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 1;
            imm8 = parse_imm8(&opcode[16]);
            matches++;
            //instr = (uintptr_t) &JMP_MemNear;
            sprintf(full_instr_name, "JMP MemNear");
        } else if (width == 32 && (memcmp(oo, "10", 2) == 0
                || (w == '1' && memcmp(oo, "00", 2) == 0))) {
            imm_type = 2;
            imm16 = parse_imm16(&opcode[16]);
            matches++;
            //instr = (uintptr_t) &JMP_MemNear;
            sprintf(full_instr_name, "JMP MemNear");
        } else if (memcmp(oo, "11", 2) == 0) {
            matches++;
            //instr = (uintptr_t) &JMP_RegWord;
            sprintf(full_instr_name, "JMP RegWord");
        }
    }
    if (width >= 16 && memcmp(opcode, "11100010", 8) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        instr = (uintptr_t) &LOOP;
        LOOP(imm8);
        sprintf(full_instr_name, "LOOP");
    }
    if (width >= 16 && memcmp(opcode, "11100001", 8) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        instr = (uintptr_t) &LOOPZ;
        LOOPZ(imm8);
        sprintf(full_instr_name, "LOOPZ/LOOPE");
    }
    if (width >= 16 && memcmp(opcode, "11100000", 8) == 0) {
        imm_type = 1;
        imm8 = parse_imm8(&opcode[8]);
        matches++;
        instr = (uintptr_t) &LOOPNZ;
        LOOPNZ(imm8);
        sprintf(full_instr_name, "LOOPNZ/LOOPNE");
    }
    if (width >= 16 && memcmp(opcode, "1101111011010101", 16) == 0) {
        matches++;
        //instr = (uintptr_t) &FUCOMPP;
        //FUCOMPP();
        sprintf(full_instr_name, "FUCOMPP");
    }
    if (memcmp(opcode, "00101110", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &CS;
        sprintf(full_instr_name, "CS");
    }
    if (memcmp(opcode, "00111110", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &DS;
        sprintf(full_instr_name, "DS");
    }
    if (memcmp(opcode, "00100110", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &ES;
        sprintf(full_instr_name, "ES");
    }
    if (memcmp(opcode, "01100100", 8) == 0) {
        matches++;
        //instr = (uintptr_t) &FS;
        sprintf(full_instr_name, "FS");
    }
    if (!matches && width >= 48) {
        printf("  Error: No match");
    } else if (matches > 1) {
        printf("  Error: Ambiguity in parsing");
    } else if (matches == 1) {
        if (show_parser_output) {
            printf("  %s", full_instr_name);
        }
        if (!instr) {
            printf("  Error: function not implemented");
            instr = (uintptr_t) &NOP;
        }
        if (show_parser_output) {
            if (w != '\0') {
                printf("  w: %c", w);
            }
            if (*oo != '\0') {
                printf("  oo: %s", oo);
            }
            if (*sss != '\0') {
                printf("  sss: %s", get_sss_name(sss));
            }
            if (*rrr != '\0') {
                printf("  rrr: %s", get_rrr_name(rrr, reg32, w));
            }
            if (*mmm != '\0') {
                printf("  mmm: %s/%s", mmm, memcmp(oo, "11", 2) ? get_mmm_name(mmm) : get_rrr_name(mmm, reg32, w));
            }
            if (*cccc != '\0') {
                printf("  cccc: %s", get_cccc_name(cccc));
            }
            if (imm_type == 1) {
                *imm_buf = imm8;
                printf("  imm8: %d", imm8);
            }
            if (imm_type == 2) {
                *imm_buf = imm16;
                printf("  imm16: %d", imm16);
            }
            if (instr == (uintptr_t) &INT) {
                printf("  AH: %d", ah);
            }
            if (imm_dat_type == 1) {
                printf("  imm_dat8: %d", imm_dat8);
            }
            if (imm_dat_type == 2) {
                printf("  imm_dat16: %d", imm_dat16);
            }
            printf("  opcode: %s", opcode);
        }
    }
    return instr;
}

int exec_prog(char* fname) {
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open input file %s\n", fname);
        exit(1);
    }
    int addr = 0;
    // The currently scanned 4-letter hexadecimal word
	char hexword[5];
	while (fscanf(fp, "%s", hexword) != EOF) {
        for (int i = 0; i < strlen(hexword); i++) {
            // Write the scanned word into main memory
            MEM[addr++] = hex_char_to_s16(hexword[i]);
        }
	}
	fclose(fp);

    // Buffer holding opcode being parsed, in string format
    char opcode[49] = {0};
    short pos = 0;
    // Now parse the program that was just loaded into memory
    uintptr_t instr = NULL;
    short imm = 0;
	s16 call_stk_start = *REG(SP);
    for (int i = 0; *REG(SP) <= call_stk_start; i++) {
	    s16 *ip=REG(IP);
        if (show_parser_output) {
            printf("MEM[%d] : ", (*REG(IP)));
        }
        for (int j = 3; j >= 0; j--) {
            if (MEM[*REG(IP)] & (1 << j)) {
                opcode[pos] = '1';
            } else {
                opcode[pos] = '0';
            }
            if (show_parser_output) {
                printf("%c", opcode[pos]);
            }
            pos++;
            // Check if we have a complete instruction
            if (pos > 0 && !(pos % 8)) {
                if ((instr = exec_instr(opcode, pos, fp, &imm)) || pos == 48) {
                    memset(opcode, 0, sizeof(opcode));
                    pos = 0;
                }
            }
        }
        if (show_parser_output) {
            printf("\n");
        }
        (*REG(IP))++;
    }
	return 0;
}
