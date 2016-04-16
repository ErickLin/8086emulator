#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
}

char* get_rrr_name(char* rrr, char reg32, char w) {
    if (memcmp(rrr, "000", 3) == 0) {
        return reg32 ? "EAX" : w ? "AX" : "AL";
    } else if (memcmp(rrr, "001", 3) == 0) {
        return reg32 ? "ECX" : w ? "CX" : "CL";
    } else if (memcmp(rrr, "010", 3) == 0) {
        return reg32 ? "EDX" : w ? "DX" : "DL";
    } else if (memcmp(rrr, "011", 3) == 0) {
        return reg32 ? "EBX" : w ? "BX" : "BL";
    } else if (memcmp(rrr, "100", 3) == 0) {
        return reg32 ? "ESP" : w ? "SP" : "AH";
    } else if (memcmp(rrr, "101", 3) == 0) {
        return reg32 ? "EBP" : w ? "BP" : "CH";
    } else if (memcmp(rrr, "110", 3) == 0) {
        return reg32 ? "ESI" : w ? "SI" : "DH";
    } else if (memcmp(rrr, "111", 3) == 0) {
        return reg32 ? "EDI" : w ? "DI" : "BH";
    }
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
    } else if (memcmp(sss, "100", 3) == 0) {
        return "FS";
    } else if (memcmp(sss, "101", 3) == 0) {
        return "GS";
    } else {
        return "INVALID";
    }
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
}

// Return 1 if opcode matches the format of some instruction, 0 if not
short exec_instr(char* opcode, short width) {
    short ret = 0;
    char reg32, w, oo[3], mmm[4], rrr[4], sss[4], cccc[5];
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
    if (width == 8) {
        printf("%s", opcode);
        if (memcmp(opcode, "00110111", 8) == 0) {
            ret++;
            printf("  AAA");
        }
        if (memcmp(opcode, "00111111", 8) == 0) {
            ret++;
            printf("  AAS");
        }
        if (memcmp(opcode, "0001010", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  ADC Acc,Imm");
        }
        if (memcmp(opcode, "0000010", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  ADD Acc,Imm");
        }
        if (memcmp(opcode, "10011000", 8) == 0) {
            ret++;
            printf("  CBW");
        }
        if (memcmp(opcode, "11111000", 8) == 0) {
            ret++;
            printf("  CLC");
        }
        if (memcmp(opcode, "11111100", 8) == 0) {
            ret++;
            printf("  CLD");
        }
        if (memcmp(opcode, "11111010", 8) == 0) {
            ret++;
            printf("  CLI");
        }
        if (memcmp(opcode, "11110101", 8) == 0) {
            ret++;
            printf("  CMC");
        }
        if (memcmp(opcode, "0011110", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  CMP Acc, Imm");
        }
        if (memcmp(opcode, "10100110", 8) == 0) {
            ret++;
            printf("  CMPSB");
        }
        if (memcmp(opcode, "10100111", 8) == 0) {
            ret++;
            printf("  CMPSW");
        }
        if (memcmp(opcode, "10011001", 8) == 0) {
            ret++;
            printf("  CWD");
        }
        if (memcmp(opcode, "00100111", 8) == 0) {
            ret++;
            printf("  DAA");
        }
        if (memcmp(opcode, "00101111", 8) == 0) {
            ret++;
            printf("  DAS");
        }
        if (memcmp(opcode, "01001", 5) == 0) {
            memcpy(rrr, &opcode[5], 3);
            ret++;
            printf("  DEC RegWord");
        }
        if (memcmp(opcode, "11110100", 8) == 0) {
            ret++;
            printf("  HALT");
        }
        if (memcmp(opcode, "1110010", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  IN Acc,Imm8");
        }
        if (memcmp(opcode, "1110110", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  IN Acc,DX");
        }
        if (memcmp(opcode, "01000", 5) == 0) {
            memcpy(rrr, &opcode[5], 3);
            ret++;
            printf("  INC RegWord");
        }
        if (memcmp(opcode, "11001100", 8) == 0) {
            ret++;
            printf("  INT 3");
        }
        if (memcmp(opcode, "11001101", 8) == 0) {
            ret++;
            printf("  INT Imm8");
        }
        if (memcmp(opcode, "11001110", 8) == 0) {
            ret++;
            printf("  INTO");
        }
        if (memcmp(opcode, "11001111", 8) == 0) {
            ret++;
            printf("  IRET");
        }
        if (memcmp(opcode, "10011111", 8) == 0) {
            ret++;
            printf("  LAHF");
        }
        if (memcmp(opcode, "10101100", 8) == 0) {
            ret++;
            printf("  LODSB");
        }
        if (memcmp(opcode, "10101101", 8) == 0) {
            ret++;
            printf("  LODSW");
        }
        if (memcmp(opcode, "1010001", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  MOV MemOfs,Acc");
        }
        if (memcmp(opcode, "1010000", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  MOV Acc,MemOfs");
        }
        if (memcmp(opcode, "1011", 4) == 0) {
            w = opcode[4];
            memcpy(rrr, &opcode[5], 3);
            ret++;
            printf("  MOV Reg,Imm");
        }
        if (memcmp(opcode, "10100100", 8) == 0) {
            ret++;
            printf("  MOVSB");
        }
        if (memcmp(opcode, "10100101", 8) == 0) {
            ret++;
            printf("  MOVSW");
        }
        if (memcmp(opcode, "10010000", 8) == 0) {
            ret++;
            printf("  NOP");
        }
        if (memcmp(opcode, "0000110", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  OR Acc,Imm");
        }
        if (memcmp(opcode, "1110011", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  OUT Imm8,Acc");
        }
        if (memcmp(opcode, "1110111", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  OUT DX,Acc");
        }
        if (memcmp(opcode, "01011", 5) == 0) {
            memcpy(rrr, &opcode[5], 3);
            ret++;
            printf("  POP RegWord");
        }
        if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "111", 3) == 0) {
            memcpy(sss, &opcode[2], 3);
            ret++;
            printf("  POP SegOld");
        }
        if (memcmp(opcode, "10011101", 8) == 0) {
            ret++;
            printf("  POPF");
        }
        if (memcmp(opcode, "01010", 5) == 0) {
            memcpy(rrr, &opcode[5], 3);
            ret++;
            printf("  PUSH RegWord");
        }
        if (memcmp(opcode, "00", 2) == 0 && memcmp(opcode + 5, "110", 3) == 0) {
            memcpy(sss, &opcode[2], 3);
            ret++;
            printf("  PUSH SegOld");
        }
        if (memcmp(opcode, "10011100", 8) == 0) {
            ret++;
            printf("  PUSHF");
        }
        if (memcmp(opcode, "11000011", 8) == 0) {
            ret++;
            printf("  RET NEAR");
        }
        if (memcmp(opcode, "11000010", 8) == 0) {
            ret++;
            printf("  RET imm NEAR");
        }
        if (memcmp(opcode, "11001011", 8) == 0) {
            ret++;
            printf("  RET FAR");
        }
        if (memcmp(opcode, "11001010", 8) == 0) {
            ret++;
            printf("  RET imm FAR");
        }
        if (memcmp(opcode, "10011110", 8) == 0) {
            ret++;
            printf("  SAHF");
        }
        if (memcmp(opcode, "10101110", 8) == 0) {
            ret++;
            printf("  SCASB");
        }
        if (memcmp(opcode, "10101111", 8) == 0) {
            ret++;
            printf("  SCASW");
        }
        if (memcmp(opcode, "11111001", 8) == 0) {
            ret++;
            printf("  STC");
        }
        if (memcmp(opcode, "11111101", 8) == 0) {
            ret++;
            printf("  STD");
        }
        if (memcmp(opcode, "11111011", 8) == 0) {
            ret++;
            printf("  STI");
        }
        if (memcmp(opcode, "10101010", 8) == 0) {
            ret++;
            printf("  STOSB");
        }
        if (memcmp(opcode, "10101011", 8) == 0) {
            ret++;
            printf("  STOSW");
        }
        if (memcmp(opcode, "0010110", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  SUB Acc,Imm");
        }
        if (memcmp(opcode, "1010100", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  TEST Acc,Imm");
        }
        if (memcmp(opcode, "10011011", 8) == 0) {
            ret++;
            printf("  WAIT");
        }
        if (memcmp(opcode, "10010", 5) == 0) {
            memcpy(rrr, &opcode[5], 3);
            ret++;
            printf("  XCHG AccWord/RegWord, RegWord/AccWord");
        }
        if (memcmp(opcode, "11010111", 8) == 0) {
            ret++;
            printf("  XLAT");
        }
        if (memcmp(opcode, "0011010", 7) == 0) {
            w = opcode[7];
            ret++;
            printf("  XOR");
        }
        if (memcmp(opcode, "11101000", 8) == 0) {
            ret++;
            printf("  CALL Near");
        }
        if (memcmp(opcode, "10011010", 8) == 0) {
            ret++;
            printf("  CALL Far");
        }
        if (memcmp(opcode, "0111", 4) == 0) {
            memcpy(cccc, &opcode[4], 4);
            ret++;
            printf("  Jcc Short");
        }
        if (memcmp(opcode, "11100011", 8) == 0) {
            ret++;
            printf("  JCXZ/JCXE/JECXZ/JECXE");
        }
        if (memcmp(opcode, "11101011", 8) == 0) {
            ret++;
            printf("  JMP Short");
        }
        if (memcmp(opcode, "11101001", 8) == 0) {
            ret++;
            printf("  JMP Near");
        }
        if (memcmp(opcode, "11101010", 8) == 0) {
            ret++;
            printf("  JMP Far");
        }
        if (memcmp(opcode, "11100010", 8) == 0) {
            ret++;
            printf("  LOOP");
        }
        if (memcmp(opcode, "11100001", 8) == 0) {
            ret++;
            printf("  LOOPZ/LOOPE");
        }
        if (memcmp(opcode, "11100000", 8) == 0) {
            ret++;
            printf("  LOOPNZ/LOOPNE");
        }
        if (memcmp(opcode, "11110000", 8) == 0) {
            ret++;
            printf("  LOCK");
        }
        if (memcmp(opcode, "11110011", 8) == 0) {
            ret++;
            printf("  REP/REPE/REPZ");
        }
        if (memcmp(opcode, "11110010", 8) == 0) {
            ret++;
            printf("  REPNE/REPNZ");
        }
        if (memcmp(opcode, "00101110", 8) == 0) {
            ret++;
            printf("  CS");
        }
        if (memcmp(opcode, "00111110", 8) == 0) {
            ret++;
            printf("  DS");
        }
        if (memcmp(opcode, "00100110", 8) == 0) {
            ret++;
            printf("  ES");
        }
        if (memcmp(opcode, "00110110", 8) == 0) {
            ret++;
            printf("  SS");
        }
    } else if (width == 16) {
        printf("%s", opcode);
        if (memcmp(opcode, "1101010100001010", 16) == 0) {
            ret++;
            printf("  AAD");
        }
        if (memcmp(opcode, "1101010000001010", 16) == 0) {
            ret++;
            printf("  AAM");
        }
        if (memcmp(opcode, "0001001", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADC Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0001000", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADC Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADC Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADC Reg/Mem,Imm");
        }
        if (memcmp(opcode, "0000001", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADD Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0000000", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADD Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADD Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ADD Reg/Mem,Imm");
        }
        if (memcmp(opcode, "0010001", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  AND Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0010000", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  AND Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  AND Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  AND Reg/Mem,Imm");
        }
        if (memcmp(opcode, "0011101", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CMP Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0011100", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CMP Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CMP Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CMP Reg/Mem,Imm");
        }
        if (memcmp(opcode, "1111111", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  DEC Reg/Mem");
        }
        if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  DIV Reg/Mem");
        }
        if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  IDIV Reg/Mem");
        }
        if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  IMUL Reg/Mem");
        }
        if (memcmp(opcode, "1111111", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  INC Reg/Mem");
        }
        if (memcmp(opcode, "11000101", 8) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  LDS Reg16,Mem32");
        }
        if (memcmp(opcode, "11000100", 8) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  LES Reg16,Mem32");
        }
        if (memcmp(opcode, "10001101", 8) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  LEA Reg16,Mem32");
        }
        if (memcmp(opcode, "1100011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  MOV Mem/Imm");
        }
        if (memcmp(opcode, "1000101", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  MOV Reg,Reg/Mem");
        }
        if (memcmp(opcode, "1000100", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  MOV Mem,Reg");
        }
        if (memcmp(opcode, "10001100", 8) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(sss, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  MOV Reg16/Mem16,Seg");
        }
        if (memcmp(opcode, "10001110", 8) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(sss, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  MOV Seg,Reg16/Mem16");
        }
        if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  MUL Reg/Mem");
        }
        if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  NEG Reg/Mem");
        }
        if (memcmp(opcode, "0000101", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  OR Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0000100", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  OR Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  OR Reg/Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  OR Reg/Imm");
        }
        if (memcmp(opcode, "10001111", 8) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  POP MemWord");
        }
        if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  PUSH MemWord");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  RCL Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  RCL Reg/Mem,CL");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  RCR Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  RCR Reg/Mem,CL");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ROL Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ROL Reg/Mem,CL");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ROR Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "001", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  ROR Reg/Mem,CL");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SAL/SHL Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SAL/SHL Reg/Mem,CL");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SAR Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "111", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SAR Reg/Mem,CL");
        }
        if (memcmp(opcode, "1101000", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SHR Reg/Mem,1");
        }
        if (memcmp(opcode, "1101001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SHR Reg/Mem,CL");
        }
        if (memcmp(opcode, "0001101", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SBB Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0001100", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SBB Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SBB Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SBB Reg/Mem,Imm");
        }
        if (memcmp(opcode, "0010101", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SUB Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0010100", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SUB Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SUB Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  SUB Reg/Mem,Imm");
        }
        if (memcmp(opcode, "1000010", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  TEST Reg/Mem,Reg/Mem");
        }
        if (memcmp(opcode, "1111011", 7) == 0 && memcmp(opcode + 10, "000", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  TEST Reg/Mem,Imm");
        }
        if (memcmp(opcode, "1000011", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  XCHG Reg/Mem,Reg/Mem");
        }
        if (memcmp(opcode, "0011001", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  XOR Reg,Reg/Mem");
        }
        if (memcmp(opcode, "0011000", 7) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(rrr, &opcode[10], 3);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  XOR Mem,Reg");
        }
        if (memcmp(opcode, "1000001", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  XOR Reg/Mem,Imm8");
        }
        if (memcmp(opcode, "1000000", 7) == 0 && memcmp(opcode + 10, "110", 3) == 0) {
            w = opcode[7];
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  XOR Reg/Mem,Imm");
        }
        if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "011", 3) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CALL MemFar");
        }
        if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "010", 3) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CALL RegWord/MemNear");
        }
        if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "101", 3) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  JMP MemFar");
        }
        if (memcmp(opcode, "11111111", 8) == 0 && memcmp(opcode + 10, "100", 3) == 0) {
            memcpy(oo, &opcode[8], 2);
            memcpy(mmm, &opcode[13], 3);
            ret++;
            printf("  CALL RegWord/MemNear");
        }
        if (!ret) {
            printf("  Error: No match");
        }
    }
    if (ret > 1) {
        printf("  Error: Ambiguity in parsing");
    }
    if (*oo != '\0') {
        if (memcmp(oo, "00", 2) == 0) {
        } else if (memcmp(oo, "01", 2) == 0) {
        } else if (memcmp(oo, "10", 2) == 0) {
        } else if (memcmp(oo, "11", 2) == 0) {
        }
    }
    if (*mmm != '\0') {
        printf("  mmm: %s", get_mmm_name(mmm), mmm);
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
    return ret;
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
    char opcode[17] = {0};
    short pos = 0;
    // The currently scanned 4-letter hexadecimal word
	char hexword[4];
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
                    if (exec_instr(opcode, pos) || pos == 16) {
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

int main() {
	//exec("DOSDEF.COM");
	exec("hello.com");
	exit(0);
}
